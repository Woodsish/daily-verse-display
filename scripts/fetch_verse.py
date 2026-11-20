import os
import io
import json
import hashlib
from datetime import datetime
from zoneinfo import ZoneInfo

import requests
from bs4 import BeautifulSoup
from PIL import Image, ImageFilter

# Screen size (landscape, matches DISPLAY_ROTATION = 1 in display.cpp)
SCREEN_W, SCREEN_H = 480, 320

# Get secrets from environment variables
YVP_APP_KEY = os.environ.get("YVP_APP_KEY")
BIBLE_ID = os.environ.get("BIBLE_ID", "3034")  # Default to BSB if not set

if not YVP_APP_KEY:
    raise ValueError("Missing YVP_APP_KEY environment variable!")

headers = {
    "X-YVP-App-Key": YVP_APP_KEY,
    "Accept": "application/json"
}

# Today's date and day of the year (Pacific time)
now = datetime.now(ZoneInfo("America/Los_Angeles"))
day_of_year = now.timetuple().tm_yday
date_string = now.strftime("%Y-%m-%d")


def get_verse_of_the_day():
    # Get the passage ID for today from YouVersion
    votd_url = f"https://api.youversion.com/v1/verse-of-the-days/{day_of_year}"

    try:
        response = requests.get(votd_url, headers=headers)
    except requests.RequestException as e:
        print(f"Network error fetching VOTD mapping: {e}")
        return

    if response.status_code != 200:
        print(f"Failed to fetch VOTD mapping ({response.status_code}): {response.text}")
        return

    passage_id = response.json().get("passage_id")

    # Fetch the scripture text for that passage
    passage_url = f"https://api.youversion.com/v1/bibles/{BIBLE_ID}/passages/{passage_id}"
    try:
        text_response = requests.get(passage_url, headers=headers, params={"format": "text"})
    except requests.RequestException as e:
        print(f"Network error fetching passage: {e}")
        return

    if text_response.status_code != 200:
        print(f"Failed to fetch scripture text ({text_response.status_code}): {text_response.text}")
        return

    passage_data = text_response.json()

    return {
        "date": date_string,
        "reference": passage_id,
        "version": "BSB",
        "text": passage_data.get("content"),
        "image": "today.jpg",
    }


def convert_for_display(raw_bytes, blur_radius=20):
    """Convert an image to a screen-sized JPEG with a blurred image background.

    The background fills the screen while the original image scales to fit
    without cropping, keeping any embedded text intact.
    """
    source = Image.open(io.BytesIO(raw_bytes)).convert("RGB")

    background_scale = max(SCREEN_W / source.width, SCREEN_H / source.height)
    background_size = (
        round(source.width * background_scale),
        round(source.height * background_scale),
    )
    background = source.resize(background_size, Image.LANCZOS)
    left = (background.width - SCREEN_W) // 2
    top = (background.height - SCREEN_H) // 2
    background = background.crop((left, top, left + SCREEN_W, top + SCREEN_H))
    canvas = background.filter(ImageFilter.GaussianBlur(blur_radius))

    foreground_scale = min(SCREEN_W / source.width, SCREEN_H / source.height)
    foreground_size = (
        round(source.width * foreground_scale),
        round(source.height * foreground_scale),
    )
    foreground = source.resize(foreground_size, Image.LANCZOS)
    canvas.paste(
        foreground,
        ((SCREEN_W - foreground_size[0]) // 2, (SCREEN_H - foreground_size[1]) // 2),
    )

    out = io.BytesIO()
    canvas.save(out, "JPEG", quality=85, progressive=False, optimize=False)
    return out.getvalue()


# Note: the image of the day has the text embedded in the image.
def get_image_of_the_day():
    page_url = "https://www.bible.com/verse-of-the-day"

    web_headers = {
        "User-Agent": "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/120.0.0.0 Safari/537.36",
        "Accept": "text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8",
        "Accept-Language": "en-US,en;q=0.5"
    }

    try:
        response = requests.get(page_url, headers=web_headers)
        response.raise_for_status()

        # Find the image link in the page metadata
        soup = BeautifulSoup(response.content, "html.parser")
        image_tag = (
            soup.find("meta", attrs={"property": "og:image"}) or
            soup.find("meta", attrs={"name": "twitter:image"})
        )

        if not (image_tag and image_tag.get("content")):
            print("Could not find the image URL in the page metadata.")
            return

        img_response = requests.get(image_tag["content"], headers=web_headers)
        img_response.raise_for_status()

        # Convert to a baseline, screen-sized JPEG and save it
        os.makedirs("data", exist_ok=True)
        with open("data/today.jpg", "wb") as img_file:
            img_file.write(convert_for_display(img_response.content))

        # Hash of the original download, used for change detection
        return hashlib.md5(img_response.content).hexdigest()[:8]

    except Exception as e:
        print(f"Error fetching the image of the day: {e}")


if __name__ == "__main__":
    votd_data = get_verse_of_the_day()
    img_hash = get_image_of_the_day()

    if votd_data:
        votd_data["image_hash"] = img_hash or ""
        os.makedirs("data", exist_ok=True)
        with open("data/today.json", "w") as json_file:
            json.dump(votd_data, json_file, indent=4)