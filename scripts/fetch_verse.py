import os
from datetime import datetime
import requests
import json
from bs4 import BeautifulSoup

# Get secretes from environment variables
# YVP_APP_KEY = os.environ.get("YVP_APP_KEY")
YVP_APP_KEY = "dJUUAAmVTs87ARaq6rVKZ8B77w3Uj16aFEpFAdpLGEMU2Dpy"
BIBLE_ID = os.environ.get("BIBLE_ID", "3034")  # Default to BSB if not set

if not YVP_APP_KEY:
    raise ValueError("Missing YVP_APP_KEY environment variable!")

headers = {
    "X-YVP-App-Key": YVP_APP_KEY,
    "Accept": "application/json"
}

# Calculate the day of the year
day_of_year = datetime.now().astimezone().timetuple().tm_yday
date_string = datetime.now().astimezone().strftime("%Y-%m-%d")

def get_verse_of_the_day():
    # Get the passage ID for today from YouVersion
    votd_url = f"https://api.youversion.com/v1/verse-of-the-days/{day_of_year}"
    print(f"Requesting URL: {votd_url}")

    try:
        response = requests.get(votd_url, headers=headers)
    except requests.RequestException as e:
        print(f"Network error trying to resolve host: {e}")
        return
    print(f"VOTD Status Code: {response.status_code}")

    if response.status_code != 200:
        print(f"Failed to fetch VOTD mapping: {response.text}")
        return

    data = response.json()
    passage_id = data.get("passage_id")
    print(f"Passage ID: {passage_id}")

    # Build the specific passage URL
    passage_url = f"https://api.youversion.com/v1/bibles/{BIBLE_ID}/passages/{passage_id}"
    params ={"format": "text"}
    print(f"Requesting passage URL: {passage_url}")

    # Fetch the scripture for the verse of the day
    try:
        text_response = requests.get(passage_url, headers=headers, params=params)
    except requests.RequestException as e:
        print(f"Network error trying to reach passages endpoint: {e}")
        return
    print(f"Scripture Text Status Code: {text_response.status_code}")

    if text_response.status_code != 200:
        print(f"Failed to fetch scripture text: {text_response.text}")
        return
    passage_data = text_response.json()

    # Format the verse of the day data
    votd_data = {
        "date": date_string,
        "reference": passage_id,
        "version": "BSB",
        "text": passage_data.get('content'),
        "image": "today.jpg",
    }

    # Test Outputs
    print("\n========================================")
    print(f"YOUVERSION VERSE OF THE DAY (BSB) - {votd_data['date']}")
    print("========================================")
    print(f"Reference: {votd_data['reference']}")
    print(f"Version: {votd_data['version']}")
    print(f"Text: {votd_data['text']}")
    print("========================================")

    # Save the verse of the day to the data file
    json_file_path = "data/today.json"
    with open(json_file_path, "w") as json_file:
        json.dump(votd_data, json_file, indent=4)

# Note: the image of the day has the text embedded in the image.
def get_image_of_the_day():
    votd_url = "https://www.bible.com/verse-of-the-day"

    headers = {
        "User-Agent": "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/120.0.0.0 Safari/537.36",
        "Accept": "text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8",
        "Accept-Language": "en-US,en;q=0.5"
    }

    try:
        print("Connecting to Bible.com...")
        response = requests.get(votd_url, headers=headers)
        response.raise_for_status()

        # Parse the HTML content
        soup = BeautifulSoup(response.content, "html.parser")

        # Locate the image link embedded in the page metadata
        image_tag = (
            soup.find("meta", attrs={"property": "og:image"}) or 
            soup.find("meta", attrs={"name": "twitter:image"})
        )

        if image_tag and image_tag.get("content"):
            image_url = image_tag["content"]
            print(f"Found today's image: {image_url}")

            # Fetch the image
            img_response = requests.get(image_url, headers=headers)
            img_response.raise_for_status()

            image_file_path = "data/today.jpg"
            with open(image_file_path, "wb") as img_file:
                img_file.write(img_response.content)
            print(f"Saved image to: {image_file_path}")
        else:
            print("Could not parse dynamic graphic element container from metadata tags.")
    
    except Exception as e:
        print(f"Error occured while tracking the image: {e}")

if __name__ == "__main__":
    get_verse_of_the_day()
    get_image_of_the_day()