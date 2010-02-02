@ECHO OFF
make
SET COUNT=1
:Loop
del qn.bin
rl-learner > %1-%COUNT%.txt
echo.
SET /A COUNT=%COUNT%+1
If %COUNT% GTR %2 goto Exit
Goto Loop
:Exit
avgdatafiles %1- > %1.txt
del %1-*

