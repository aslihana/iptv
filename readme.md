# IPTV M3U Parser in C

A simple C-based IPTV playlist parser that fetches a remote `.m3u` playlist, extracts channel names and stream URLs, and prints them in a plain text or CSV-friendly format.

## Overview

This project downloads an IPTV playlist from a given URL, parses the `#EXTINF` entries, and matches each channel name with its corresponding stream URL.

It is designed to be lightweight and easy to use without needing Python, virtual environments, or extra scripting layers.

## Features

- Written in C
- Fetches remote M3U playlists over HTTP/HTTPS
- Extracts channel names and stream URLs
- Simple command-line usage
- Output can be redirected into a `.csv` file
- Suitable as a base for a larger IPTV updater project

## Requirements

- GCC
- `libcurl`

## Build

Compile the program with:

```bash
gcc iptv.c -o iptv -lcurl
```

## Usage
Run the parser with an M3U playlist URL:

```bash
./iptv "https://iptv-org.github.io/iptv/categories/science.m3u"
```

## Save Output to a File
You can redirect the output into a file:
```bash
./iptv "https://iptv-org.github.io/iptv/categories/science.m3u" > science.csv
```

## Example

```bash
./iptv "https://iptv-org.github.io/iptv/categories/culture.m3u" > culture.csv
./iptv "https://iptv-org.github.io/iptv/categories/news.m3u" > news.csv
./iptv "https://iptv-org.github.io/iptv/categories/sports.m3u" > sports.csv
```

## Input Format

This parser expects a standard M3U playlist format such as:

#EXTM3U
#EXTINF:-1 tvg-id="example" tvg-name="Example Channel",Example Channel
http://example.com/live/stream.m3u8

The parser reads:

the channel name from the #EXTINF line
the stream URL from the next non-comment line

## Notes

The source playlist URL may stay stable even if the stream links inside it change over time.
This makes the parser useful for refreshing channel data from static category playlists such as those from IPTV-org.
The current workflow is well suited for generating updated channel lists that can later be committed into a GitHub repository.

## Current Workflow

A typical local workflow looks like this:

- Compile the parser
- Run it against a category playlist
- Redirect the output into a file

Example:

```bash
gcc iptv.c -o iptv -lcurl
./iptv "https://iptv-org.github.io/iptv/categories/science.m3u" > science.csv
```

## Future Ideas

Possible future improvements for this project:

- JSON output
- Proper CSV escaping
- Support for multiple playlist URLs in one run
- Automatic file naming based on category name
- GitHub Actions integration for scheduled updates
- Filtering by group or metadata
- Duplicate removal
- Dead link checking
- Disclaimer

This tool only parses playlist data and prints the extracted channel names and URLs. Availability and legality of IPTV streams depend on the source and the applicable rules in your region.
