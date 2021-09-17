Launch date: 17-09-2021

# Story
At the start of the day we just could not get a fix with the GPS module, after re-attaching the antenna of the GPS it magically started working again and we got a 24 (!) satellite fix. The launch went really well, except that it was really hard to get data while the rocket fairing was closed, so thats why there is a sharp line on the ascent. Somehow, magically, the cansat survived the impact too, and the GPS coördinates were accurate enough that the military personnel were able to retreive the cansat perfectly on the given last coördinates. The cansat stopped transmitting on impact, however, the Arduino was still operational when we got it back in our hands.

# Received data (Launch Day)
Data Plots:
![image](https://user-images.githubusercontent.com/25268098/133812163-57eb1e61-c164-4e92-8f94-41eb4e3889fe.png)
The temperature graph is a little screwed, the wire of the temperature sensor was a bit too short so it couldn't be fitted externally like original design.

- Average wind speed from start of descent: 3.1085334171646997 ms^-1.
- Average descent speed:  13.19164960342676  ms^-1.
- Terminal velocity:  13.096136803754872 ms^-1 (Almost too slow!)

### 2d plot:
![image](https://user-images.githubusercontent.com/25268098/133815980-03e516cc-0f64-458d-9644-ecbdffdf16cd.png)

### 3d plot:
![image](https://user-images.githubusercontent.com/25268098/133816271-5a08645c-8c94-4f89-ad9a-e00f418c2b4d.png)
We were missing some data points on ascent, this is because the cansat was in a metal cage and receiving data was very hard.

