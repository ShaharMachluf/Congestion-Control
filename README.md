# Congestion-Control
This project was about examining the differences between cubic and reno.  
We created two c files:  
- Sender: sends the file 5 times using cubic and then sending it 5 times using reno.
- Measure: measure the time it takes from the time it requested to get the file to the time it receives the file for the 5th time using cubic and then does the same using reno.

For further eximination I recommend using wireshark at the same time to capture the packets and seeing the difference between the algorithms (cubic and reno).
