# IP Information Tool

A **command-line IP information tool** written in C. Fetch detailed information about one or multiple IP addresses, including geolocation, ISP/organization, timezone, and network info. Supports **automatic public IP detection**, **colorized terminal output**, and **raw JSON output** for scripting.

---

## Features

- **Fetch IP information**  
  Get details for IPv4 addresses using [ipinfo.io](https://ipinfo.io/): city, region, country, ISP/organization, timezone, and geolocation.

- **Automatic public IP detection**  
  Use `my_ip` argument to automatically fetch information for your current public IP via [ident.me](https://ident.me).

- **Colorized terminal output**  
  JSON keys, values, and headers are colorized for easy readability.

- **Raw JSON output**  
  Use `--json` to get the full JSON response for automation or scripting.

- **Help message**  
  `-h` or `--help` displays a colorized usage guide.

- **Multiple IPs**  
  Supports multiple IP addresses in a single command.

---

## Dependencies

- `libcurl` – for HTTP requests  
- `cJSON` – for parsing JSON data  
- ANSI-compatible terminal – for colorized output  

Install on Debian/Ubuntu:

```bash
sudo apt update
sudo apt install libcurl4-openssl-dev libcjson-dev gcc
```

Install on RedHat/Fedora:
```bash
sudo dnf install libcurl-devel cjson-devel gcc   # Fedora
sudo yum install libcurl-devel cjson-devel gcc   # RHEL
```

## Compilation
```
gcc ip_info_auto.c -o ip_info_auto -lcurl -lcjson
```
Usage

Output for specific IPs:
```
./ip_info_auto 8.8.8.8 1.1.1.1
```
Detect your public IP and output it:
```
./ip_info_auto my_ip
```
Raw JSON output for multiple IPs:
```
./ip_info_auto --json 8.8.8.8 1.1.1.1
```
Show help:
```
./ip_info_auto -h
```
Example Output
```
=== IP INFORMATION for 8.8.8.8 ===
IP:        8.8.8.8
City:      Mountain View
Region:    California
Country:   US
Org/ISP:   AS15169 Google LLC
Timezone:  America/Los_Angeles
Location:  37.4056,-122.0775
```
Raw JSON (--json):
```
{
  "ip": "8.8.8.8",
  "city": "Mountain View",
  "region": "California",
  "country": "US",
  "loc": "37.4056,-122.0775",
  "org": "AS15169 Google LLC",
  "timezone": "America/Los_Angeles"
}
```
### License

This project is released under the GPLv3 License.
```
Use Cases

    Quickly check geolocation or ISP of an IP.

    Automate IP information collection in scripts.

    Educational tool for learning HTTP requests and JSON parsing in C.
