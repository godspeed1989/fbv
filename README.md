## fbv
A framebuffer image viewer.

Fork from http://s-tech.elsat.net.pl/fbv/ 

GIF support through libungif has been removed as that library is obsolete.

# OVERVIEW   
  fbv (FrameBuffer Viewer) is a simple program to view pictures on a linux framebuffer device. In 2000, when fbv was created, there were no other situable programs performing the same task, so the authors decided to follow the rule: `If you need a tool - write it yourself!` :-)

# REQUIREMENTS

* Linux, configured to provide the framebuffer device interface
* libjpeg for JPEG support
* libpng for PNG support
* built-in BMP support

# INSTALLATION
  
```
./configure
make
make install
```

# USAGE   
  Just run `./fbv` without any arguments, and a short help message will appear.

# AUTHORS   
* Tomasz 'smoku' Sterna <tomek@smoczy.net>
* Mateusz 'mteg' Golicz <mtg@elsat.net.pl>
  
  Feel free to send any comments, patches, bugfixes, suggestions, etc. The authors are not native english speakers, and they are aware of the fact that their english is far from perfect. Because of that, reports on grammar and vocabulary mistakes in this file are also welcome.
