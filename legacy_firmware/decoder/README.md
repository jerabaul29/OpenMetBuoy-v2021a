The **decoder.py** file contains the binary decoder; can be used either as a python package to import, or as a standalone. Requires python3 and a number of packages (see the import list at the start of the file and ```pip3 install``` as needed). Can be added to your ```${HOME}/bin``` folder, with relevant execution permissions, to be made available system wide.

The decoder expects to be provided with the hex-encoded binary messages as provided by the Rock7 Iridium provider (for example, ```470946069b13627496ba238fb7630646ff9313622396ba2375b6630646f68c13625297ba23b2b4630646ee8513626093ba23c6b5630645``` is such a message).

- Examples of use from console:

```
$ python3 decoder.py -h
Usage: decoder.py [OPTIONS]

  Simple CLI for decoding tracker messages. This can be added as a command
  by copying into $HOME/bin and making executable. (you may need to $ source
  ~/.profile to activate)

Options:
  -e, --example          Example of installation and use
  -m, --module           Show how to use as a module
  -t, --autotest         Run a few autotests to check that the package works
  -s, --version          Print the version number
  -v, --verbose          Turn on verbosity
  -b, --verbose-debug    Turn on verbosity to debug level
  -d, --decode-hex TEXT  Decode the provided hex message
  -p, --plot             Plot the data contained in the message
  -h, --help             Show this message and exit.

$ python3 decoder.py -vd 470946069b13627496ba238fb7630646ff9313622396ba2375b6630646f68c13625297ba23b2b4630646ee8513626093ba23c6b5630645
----------------------- START DECODE GNSS MESSAGE -----------------------
expected number of packets based on message length: 4
number of fixes since boot at message creation: 9
-------------------- decoded GNSS packet ---------------------
fix at posix 1645452038, i.e. 2022-02-21 14:00:38
latitude 599430772, i.e. 59.9430772
longitude 107198351, i.e. 10.7198351
--------------------------------------------------------------
-------------------- decoded GNSS packet ---------------------
fix at posix 1645450239, i.e. 2022-02-21 13:30:39
latitude 599430691, i.e. 59.9430691
longitude 107198069, i.e. 10.7198069
--------------------------------------------------------------
-------------------- decoded GNSS packet ---------------------
fix at posix 1645448438, i.e. 2022-02-21 13:00:38
latitude 599430994, i.e. 59.9430994
longitude 107197618, i.e. 10.7197618
--------------------------------------------------------------
-------------------- decoded GNSS packet ---------------------
fix at posix 1645446638, i.e. 2022-02-21 12:30:38
latitude 599429984, i.e. 59.9429984
longitude 107197894, i.e. 10.7197894
--------------------------------------------------------------
----------------------- DONE DECODE GNSS MESSAGE -----------------------
```

- Examples of use from python3:

```
$ ipython3
Python 3.8.10 (default, Nov 26 2021, 20:14:08)
Type 'copyright', 'credits' or 'license' for more information
IPython 7.13.0 -- An enhanced Interactive Python. Type '?' for help.

In [1]: from decoder import decode_message

In [2]: kind, metadata, list_packets = decode_message("470946069b13627496ba238fb7630646ff9313622396ba2375b6630646f68c13625297ba23b2b4630646ee8513626093ba23c6b5630645")
----------------------- START DECODE GNSS MESSAGE -----------------------
expected number of packets based on message length: 4
number of fixes since boot at message creation: 9
-------------------- decoded GNSS packet ---------------------
fix at posix 1645452038, i.e. 2022-02-21 14:00:38
latitude 599430772, i.e. 59.9430772
longitude 107198351, i.e. 10.7198351
--------------------------------------------------------------
-------------------- decoded GNSS packet ---------------------
fix at posix 1645450239, i.e. 2022-02-21 13:30:39
latitude 599430691, i.e. 59.9430691
longitude 107198069, i.e. 10.7198069
--------------------------------------------------------------
-------------------- decoded GNSS packet ---------------------
fix at posix 1645448438, i.e. 2022-02-21 13:00:38
latitude 599430994, i.e. 59.9430994
longitude 107197618, i.e. 10.7197618
--------------------------------------------------------------
-------------------- decoded GNSS packet ---------------------
fix at posix 1645446638, i.e. 2022-02-21 12:30:38
latitude 599429984, i.e. 59.9429984
longitude 107197894, i.e. 10.7197894
--------------------------------------------------------------
----------------------- DONE DECODE GNSS MESSAGE -----------------------

In [3]: kind
Out[3]: 'G'

In [4]: metadata
Out[4]: GNSS_Metadata(nbr_gnss_fixes=9)

In [5]: list_packets
Out[5]:
[GNSS_Packet(datetime_fix=datetime.datetime(2022, 2, 21, 14, 0, 38), latitude=59.9430772, longitude=10.7198351, is_valid=True),
 GNSS_Packet(datetime_fix=datetime.datetime(2022, 2, 21, 13, 30, 39), latitude=59.9430691, longitude=10.7198069, is_valid=True),
 GNSS_Packet(datetime_fix=datetime.datetime(2022, 2, 21, 13, 0, 38), latitude=59.9430994, longitude=10.7197618, is_valid=True),
 GNSS_Packet(datetime_fix=datetime.datetime(2022, 2, 21, 12, 30, 38), latitude=59.9429984, longitude=10.7197894, is_valid=True)]
```
