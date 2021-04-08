Gameplay recording:
Program records each move taking in the game. User can the choose to playback the last game.
This could be done with delays for smooth playback. Or, user could step through each move pressing return to
advance (but that sounds tedious).
The recording part shouldn't be hard to do, we will need dynamic memory allocation. Probably reallocation will also
come in handy. The playback part with timing is what I'm not sure about right now. Additionally I wonder whether
you could allow user to choose at what speed the playback goes.


Improvement: More consistent prompts and user input
The prompts and user input could use some improvement. Both menu.c and userInput.c could be improved.
Input validation works well, so that's a plus.


Logs
Write errors into a log file. Once this is implemented, you can change the error prints to something more suitable
for "normal" users.


Error code
Make a table of error codes and adjust functions accordingly.

AI Player
Create a computer opponent if you happen to have tons of spare time.
