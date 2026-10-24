import os
import datetime
import requests
import json

# Get secretes from environment variables
YVP_APP_KEY = os.environ.get("YVP_APP_KEY")
BIBLE_ID = os.environ.get("BIBLE_ID", "3034")  # Default to BSB if not set

if not YVP_APP_KEY:
    raise ValueError("Missing YVP_APP_KEY environment variable!")

headers = {
    "X-YVP-App-Key": YVP_APP_KEY,
    "Accept": "application/json"
}

def get_verse_of_the_day():
    # Calculate the day of the year
    day_of_year = datetime.datetime.now().timetuple().tm_yday
    print(f"Day of the year: {day_of_year}")

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
        "date": datetime.datetime.today().strftime("%Y-%m-%d"),
        "reference": passage_id,
        "version": "BSB",
        "content": passage_data.get('content'),
        "image": "today.jpg",
    }

    # Test Outputs
    print("\n========================================")
    print(f"YOUVERSION VERSE OF THE DAY (BSB) - {votd_data['date']}")
    print("========================================")
    print(f"Reference: {votd_data['reference']}")
    print(f"Version: {votd_data['version']}")
    print(f"Content: {votd_data['content']}")
    print("========================================")

    # Save the verse of the day to the data file
    json_file_path = "data/today.json"
    with open(json_file_path, "w") as json_file:
        json.dump(votd_data, json_file, indent=4)

if __name__ == "__main__":
    get_verse_of_the_day()