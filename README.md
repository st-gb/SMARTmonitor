
[comment]: # https://stackoverflow.com/questions/4823468/comments-in-markdown
[comment]: # "With an empty line before the comment."
[comment]: # Line breaks may appear after "(" for a link as this is the (only)
[comment]: # possibility to break the line at 80 characters and avoid spaces in
[comment]: # links.

this file is based on
https://gist.github.com/PurpleBooth/109311bb0361f32d87a2#file-readme-template-md

# S.M.A.R.T. Values Monitor/Processor

## Capabilities

### Monitoring Directly/Locally or via Network/Internet
A tool to monitor [normalized](
http://en.wikipedia.org/wiki/Self-Monitoring,_Analysis_and_Reporting_Technology#ATA_S.M.A.R.T._attributes)
and [raw](
http://en.wikipedia.org/wiki/Self-Monitoring,_Analysis_and_Reporting_Technology#ATA_S.M.A.R.T._attributes)
values for (critical) [S.M.A.R.T.](http://en.wikipedia.org/wiki/S.M.A.R.T.)
parameters directly/locally (only Linux operating system) or as [BSD sockets](
http://en.wikipedia.org/wiki/Berkeley_sockets) client (Microsoft Windows, Linux
operating systems).
S.M.A.R.T. values are retrieved periodically in an interval.

[comment]: # https://stackoverflow.com/questions/10189356/how-to-add-screenshot-to-readmes-in-github-repository
[comment]: # ![here the screenshot of wxWidgets GUI in Linux Mint should appear](/screenshots/SMARTmonitor_wxGUI_main_dialog.png?raw=true "screenshot of wxWidgets GUI's main dialog in Linux Mint")
![here the screenshot of wxWidgets GUI in Linux Mint should appear](http://it-gebauer.de/SMARTmonitor/SMARTmonitor_wxGUI_main_dialog_Linux_Mint.png "screenshot of wxWidgets GUI's main dialog in Linux Mint")

### S.M.A.R.T. Parameter Value Unit Auto-Detection
For some [S.M.A.R.T. attributes](
http://en.wikipedia.org/wiki/S.M.A.R.T.#Known_ATA_S.M.A.R.T._attributes)
(Power-On Time, Total Data Written, ...) units are tried to be determined.

### Multiple Data Carriers
Also the software is capable to monitor more than 1 storage media/data carrier
at the same time.

### Usage Of Appropriate Values
Basically this application uses the normalized current values for detecting how
critical the corresponding S.M.A.R.T. parameter is as rating.
But in some cases, for example the temperature, it is more senseful to rely on
raw values. Because some manufacturers don't follow the convention that lower
normalized current values for a S.M.A.R.T. parameter mean a worse state.

## Debian(-based) Linux

For Debian(-based) Linux 3 packages: common files, client, service can
be created with included shell scripts.

Both GUI/client and daemon/server/service depend on the "common files" (data
files, configuration files) package.

## systemd
For systemd the service can be installed (for example using Debian package
post-installation script).

## Building

See [here](BUILD.md)

## Tests

### minClient

BSD sockets S.M.A.R.T. client with minimal dependencies on the source code.

### minSMARTmonClientDerived

BSD sockets S.M.A.R.T. client derived from class "SMARTmonitorClient".

### readAllSupportedSMART_IDsTime

Measures time to read all S.M.A.R.T. attributes supported by a certain drive. 

## Running

If the wxWidgets GUI was built with dynamic wxWidgets libraries, then their
directories must be in "Path" environment variable.

## Authors

[Stefan Gebauer, M.Sc. Comp. Sc.](https://github.com/st-gb)

## License

Copyright by Stefan Gebauer, no commercial use without grant.
If you want to use the code in a commercial product then contact the author.

The source code is also provided to enable customization of the software.

You may use the code free of charge

- to study the implementation and quality (of one of the authour's software
  products) so bugs can be reported.
- to build targets (wxWidgets GUI, Linux service [...] ) of the software in
  order to test it, especially if a binary is not provided for another
  platform (CPU, Operating System)
- in a non-commercial product if it is not in competition with an author's
  software product (repositories from http://github.com/st-gb).
  
  If in doubt which applications this affects then ask the author.

It is appreciated to reference the author (e.g. in the "about" dialog or
README.txt) if using this code. Therefore please refer to this github user page
and/or to "it-gebauer.de".
