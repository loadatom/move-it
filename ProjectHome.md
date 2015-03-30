# What is MoveIt? #
Multi-displays are very convenient for daily work. For example, one can open an IDE on one monitor and document on another monitor, so that he doesn't have to switch windows all the time. But moving a window from one monitor to another monitor can be tedious. Using a mouse is ineffective. Things will get worse if the window is maximized, in which case the window must first be restored before it can be dragged by a mouse. This project is meant to address this problem. One can register a system-wide hotkey which can be used to move the foreground window to the next monitor.

# How to use it? #
Just extract the archive in some folder and run it. You need to first type a hotkey in the edit box, and click the "set" button to register the hotkey. If the status bar indicates "success", you're done. You can now use the hotkey to move the foreground window to another monitor. Note that certain key combination is not supported, or it may fail to register the hotkey.

When the MoveIt window is minimized, it will hide. To show it, just run MoveIt again.

# System requirements #
  * Windows XP SP3 or later(Windows 2000 may be also OK, but I didn't test on it)
  * Visual C++ 2005 SP1 runtime. You can download it [here](http://www.microsoft.com/downloads/details.aspx?displaylang=en&FamilyID=200b2fd9-ae1a-4a14-984d-389c36f85647).