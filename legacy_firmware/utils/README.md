Some examples of utils to get the binary messages from Rock7 and process them. Relies on Bash and Python3. These are only coarse examples, you will need to adapt to your needs.

- **script_interact_Rock7_API.sh**: example of how to collect data from the Rock7 website, by getting a cookie, and sending a request for CSV data. A few notes:
  - be careful about your username and password! Do not leak these into the open internet. For my part, I use pass as a password manager (and this is reflected in the script). It is also possible to just provide these as a ```password.secret``` and ```username.secret``` file, if needed (do not leak them!).
  - I do not know about some documentation about the Rock7 API, so this was reverse-engineered from their website. To tune it to your needs, you can for example in firefox web browser:
    - Go to Application menu > More tools > enable web developper tools
    - Go to Rock7 website, craft the exact request you want
    - open the "Network" tab of the web developper tools
    - perform the request of CSV data
    - you will see a new GET request appearing
    - click on it to get more information, this will show you the exact request (you can also hoover on it to see it / right click on the request to copy paste it)
    - note that urls cannot use some of the special characters that have a specific meaning, so these are "escaped". See : https://www.w3schools.com/tags/ref_urlencode.asp . In particular, "/" when part of a date (not of a file path) is encoded as 2F. There are many tools to URL-encode strings, either in Bash or in Python or else; for example:
    
```
~$ urlencode "22/Feb/"
22%2FFeb%2F
```

  - the script is used to both get the data, and run the python post processing scripts

- **decoder.py**: a local copy of the decoder from https://github.com/jerabaul29/OpenMetBuoy-v2021a/tree/main/legacy_firmware/decoder , to avoid import issues

- **params.py**: information about what instruments to use over what time period

- **script_all_messages_to_dict.py**: converting the Rock7 data into a python dict for convenience

- **script_plot_trajectories_on_map.py**: plot on a map

- **script_plot_spectra.py**: show the spectra

- **utils.py**: various small utils for the other scripts
