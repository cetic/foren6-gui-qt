This is the GUI project of the lowpan diagnosis tool. It use the analyser module to gather informations about WSN through a sniffer (using the capture module)

Use this command to generate Makefiles in the current directory:
    qmake-qt4 /home/am/git/lowpan-diagnosis-tool/gui-qt/rpl_diagnosis_tool.pro -r -spec linux-g++ CONFIG+=debug

Then use
    make debug
or
    make release
to compile the project.
The default target is debug (unless you didn't specified CONFIG+=debug to qmake when generating Makefiles)

The analyzer library (libanalyser.so) must be compiled before this project.

