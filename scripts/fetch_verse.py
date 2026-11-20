import os
import io
import json
import hashlib
from datetime import datetime
from zoneinfo import ZoneInfo

import requests
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
    # Get the passage ID and English verse-art image ID for today.
    votd_url = f"https://api.youversion.com/v1/verse-of-the-days/{day_of_year}"
    response = requests.get(
        votd_url,
        headers=headers,
        params={"language_ranges[]": "en"},
        timeout=30,
    )
    response.raise_for_status()

    verse_data = response.json()
    passage_id = verse_data.get("passage_id")
    image_id = verse_data.get("image_id")
    if not isinstance(passage_id, str) or not passage_id:
        raise ValueError("YouVersion did not return a passage ID for today's verse.")
    if not isinstance(image_id, str) or not image_id:
        raise ValueError("YouVersion did not return English verse art for today's verse.")

    # Fetch the scripture text for that passage
    passage_url = f"https://api.youversion.com/v1/bibles/{BIBLE_ID}/passages/{passage_id}"
    text_response = requests.get(
        passage_url,
        headers=headers,
        params={"format": "text"},
        timeout=30,
    )
    text_response.raise_for_status()
    passage_data = text_response.json()

    return (
        {
            "date": date_string,
            "reference": passage_id,
            "version": "BSB",
            "text": passage_data.get("content"),
            "image": "today.jpg",
        },
        image_id,
    )


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


def get_image_of_the_day(image_id):
    metadata_url = f"https://api.youversion.com/v1/images/{image_id}"
    metadata_response = requests.get(metadata_url, headers=headers, timeout=30)
    metadata_response.raise_for_status()

    image_url = metadata_response.json().get("image_url")
    if not isinstance(image_url, str) or not image_url.startswith("https://"):
        raise ValueError("YouVersion did not return a valid verse-art image URL.")

    image_response = requests.get(image_url, timeout=30)
    image_response.raise_for_status()
    image_bytes = image_response.content

    # Convert to a baseline, screen-sized JPEG with the verse text preserved.
    os.makedirs("data", exist_ok=True)
    with open("data/today.jpg", "wb") as img_file:
        img_file.write(convert_for_display(image_bytes))

    return hashlib.md5(image_bytes).hexdigest()[:8]


if __name__ == "__main__":
    votd_data, image_id = get_verse_of_the_day()
    votd_data["image_hash"] = get_image_of_the_day(image_id)
    os.makedirs("data", exist_ok=True)
    with open("data/today.json", "w") as json_file:
        json.dump(votd_data, json_file, indent=4)