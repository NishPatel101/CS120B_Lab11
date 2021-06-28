                                  █▀▄▀█ ▄▀█ ▀█▀ █░█   █▀▄▀█ ▄▀█ █▀ ▀█▀ █▀▀ █▀█ █▀  
                                  █░▀░█ █▀█ ░█░ █▀█   █░▀░█ █▀█ ▄█ ░█░ ██▄ █▀▄ ▄█

𝗦𝘂𝗺𝗺𝗮𝗿𝘆:
My game is called 𝘔𝘢𝘵𝘩 𝘔𝘢𝘴𝘵𝘦𝘳𝘴. It is an endless score game, where randomly generated addition, subtraction, multiplication and division problems need to be solved within a time limit (using the keypad).

There are only natural numbers (nonnegative integers). Due to LCD limitations, I limited the addition and subtraction subtraction to three digit numbers, and the multiplication and division are limited to 16 × 16 at most.  
  
<p align="center">
<img src="https://github.com/NishPatel101/CS120B_Lab11/blob/master/Images/Breadboard.png?raw=true" width="821" height="395">  
</p>
-----------------------------------------------------------------//----------------------------------------------------------------  

𝗔𝗱𝗱𝗶𝘁𝗶𝗼𝗻𝗮𝗹 𝗙𝗲𝗮𝘁𝘂𝗿𝗲𝘀:

𝘛𝘺𝘱𝘪𝘯𝘨  
The player can choose to erase (backspace) a character when typing. This can be done using the '*' character. This does nothing when no numbers are written. Additionally, I streamlined the answer options, so if the question is '1 + 2', any of '3', '03', and '003' will be correct. Yay!

𝘚𝘤𝘰𝘳𝘦 𝘢𝘯𝘥 𝘓𝘪𝘷𝘦𝘴  
There are score and life systems. Each correct answer grants the player a point, and each incorrect answer takes a life from the player. There are 5 lives at the beginning, and once there are 0 the game is over.

𝘛𝘪𝘮𝘦𝘳 𝘵𝘰 𝘈𝘯𝘴𝘸𝘦𝘳  
There is a time limit for answering each question. As the player's score increases, this time limit decreases (begins at 7 secs, goes down to 6 secs at 7 pts, and goes down to 5 secs at 12 pts). When the time runs out for a question, the player loses a life.

𝘔𝘦𝘯𝘶𝘴 𝘢𝘯𝘥 𝘙𝘦𝘴𝘶𝘭𝘵 𝘚𝘤𝘳𝘦𝘦𝘯𝘴  
There are result screens and two menus to make it feel more like a game. There is a start menu when powering the circuit and a retry menu after losing all lives. In either menu, the player can choose to play by pressing the '#' key. There are additional results and update screens: 'Correct', 'Incorrect', 'Time's up', and 'Speed up'. These (excluding 'Speed up') also display the correct solution so the player may learn!

<p align="center">
<img src="https://github.com/NishPatel101/CS120B_Lab11/blob/master/Images/Title_Screen.png?raw=true" width="536" height="217">  
</p>

𝘔𝘶𝘴𝘪𝘤 🎵  
For liveliness, I added music!!
There is a main theme in the start menu and a "sadder" version of it for the game over theme. There are additional jingles for 'Correct', 'Incorrect', 'Time's up', and 'Speed up'. I was going to add one for the problem phase when the player is typing their answer, but I unfortunately did not have time. :(
Informal description of the music:
Correct - triumphant jingle
Incorrect - fail sound effect (trombone)
Time's up - alarm that catches you off guard
Speed up - blinking alarm (goes with display)
