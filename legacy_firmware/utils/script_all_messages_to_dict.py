
# NOTE: this is quick and dirty stuff done in a half afternoon, would need to write a nice piece of code to to this...
# NOTE: there is not data cleaning and validation performed; if a transmission is corrupted, will result in corrupted data: need to clean later!!

import pickle as pkl
import decoder
import datetime
import os
import time
import csv
import matplotlib.pyplot as plt
from params import list_instruments_and_time, list_input_files
import tqdm

# make sure we use UTC in all our work
os.environ["TZ"] = "UTC"
time.tzset()

# convenience list_instruments and start time dict
list_instruments = [entry[0] for entry in list_instruments_and_time]
dict_instruments_to_start_time = {}
for crrt_entry in list_instruments_and_time:
    dict_instruments_to_start_time[crrt_entry[0]] = crrt_entry[1]

# the dict to contain all the data
dict_data = {}

for crrt_instrument in list_instruments:
    dict_data[crrt_instrument] = {}
    dict_data[crrt_instrument]["gnss_fixes"] = []
    dict_data[crrt_instrument]["spectra"] = []

# read all the file inputs
list_csv_entries = []

for crrt_file in list_input_files:
    with open(crrt_file, mode='r') as fh:
        input_dict = csv.DictReader(fh)
        for row in input_dict:
            list_csv_entries.append(row)

# decode the data
for crrt_dict_entry in tqdm.tqdm(list_csv_entries):
    crrt_device = crrt_dict_entry["Device"][10:15]
    print(crrt_dict_entry)

    # is this an instrument we want to look at?
    if crrt_device in list_instruments:

        # ignore empty payloads, ie failed transmissions
        if (len(crrt_dict_entry["Payload"]) > 0) and (datetime.datetime.strptime(crrt_dict_entry["Date Time (UTC)"], "%d/%b/%Y %H:%M:%S") > dict_instruments_to_start_time[crrt_device]):
            # from when do we want to look at?
            crrt_start_time = dict_instruments_to_start_time[crrt_device]

            crrt_msg = crrt_dict_entry["Payload"]

            msg_kind, msg_metadata, msg_packets = decoder.decode_message(crrt_msg)

            # only consider data after start time
            if msg_packets[0].datetime_fix > crrt_start_time:
                if msg_kind == 'G':  # a GPS fix
                    for crrt_packet in msg_packets:
                        if crrt_packet not in dict_data[crrt_device]["gnss_fixes"]:
                            dict_data[crrt_device]["gnss_fixes"].append(crrt_packet)
                elif msg_kind == 'Y':  # a wave spectrum
                    for crrt_packet in msg_packets:
                        if crrt_packet not in dict_data[crrt_device]["spectra"]:
                            dict_data[crrt_device]["spectra"].append(crrt_packet)
                else:
                    raise(RuntimeError("msg kind {} unknown".format(msg_kind)))
                for crrt_packet in msg_packets:
                    pass

                if msg_kind == "W" and crrt_device != "19641":
                    raise RuntimeError("careful, instrument {} uses the old 'X' kind of wave spectrum".format(crrt_device))

# re-order the packages
for crrt_device in list_instruments:
    dict_data[crrt_device]["gnss_fixes"].sort(key=lambda x: x.datetime_fix)
    dict_data[crrt_device]["spectra"].sort(key=lambda x: x.datetime_fix)

# remove duplicates in case of bad transmission
for crrt_device in list_instruments:
    if len(dict_data[crrt_device]["gnss_fixes"]) > 0:
        res_fixes = [dict_data[crrt_device]["gnss_fixes"][0]]
        for crrt_entry in dict_data[crrt_device]["gnss_fixes"]:
            if crrt_entry.datetime_fix == res_fixes[-1].datetime_fix:
                pass
            else:
                res_fixes.append(crrt_entry)
        dict_data[crrt_device]["gnss_fixes"] = res_fixes

    if len(dict_data[crrt_device]["spectra"]) > 0:
        res_spectra = [dict_data[crrt_device]["spectra"][0]]
        for crrt_entry in dict_data[crrt_device]["spectra"]:
            if crrt_entry.datetime_fix == res_fixes[-1].datetime_fix:
                pass
            else:
                res_spectra.append(crrt_entry)
        dict_data[crrt_device]["res_spectra"] = res_spectra

# quick and dirty visualization
plt.figure()
for crrt_instrument in list_instruments:
    list_lats = [packet.latitude for packet in dict_data[crrt_instrument]["gnss_fixes"]]
    list_lons = [packet.longitude for packet in dict_data[crrt_instrument]["gnss_fixes"]]
    plt.scatter(list_lons, list_lats, label=crrt_instrument)
plt.legend()
plt.show()

for crrt_device in list_instruments:
    first = True
    for crrt_entry in dict_data[crrt_device]["gnss_fixes"]:
        print("{}: timestamp {} at lat {} long {}".format(crrt_device, crrt_entry.datetime_fix, crrt_entry.latitude, crrt_entry.longitude))
        if first:
            first = False
            last_datetime = crrt_entry.datetime_fix
        else:
            assert crrt_entry.datetime_fix > last_datetime
            last_datetime = crrt_entry.datetime_fix

# dump the data
with open("./dict_all_data.pkl", 'wb') as fh:
        pkl.dump(dict_data, fh)

