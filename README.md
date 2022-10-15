# FanController8
Disclaimer: FanController8 is so named because Atmel studio had a bug which caused my project files to corrupt periodically. The eighth version was the final one that was necessary to finish the project.

Every single PC fan controller sucks, so I made my own. In my opinion, both the software and hardware currently available commericially are terrible, and so I redid both. I designed a small circuit board which used PWM to control fans to a certain speed (measured using an analog comparator), retrieve the temperature of the case from two onboard fan headers, and communicate with a computer via an internal USB header.

This respository contains the code and PCB files for the hardware side of the project. The fan controller could connect to any computer over USB and interface with the polished, complex GUI I built using wxWidgets.

This GUI contained charts with temperature data about the computer (some provided by temperature headers on the fan controller itself and some by the system using OpenHardwareLib and a C# to managed C++ to regular C++ DLL hack). The GUI also contained graphs where data points could be added, removed, and moved around to adjust the fan curve for the fans. You could also select which temperataure to base the fan speed on for each fan in the GUI. Unfortunately the code for the GUI has been lost over time.

With this project, I gained experience with a wide variety of microcontroller peripherals (I2C, GPIO, Comparators, Timers/PWM, USB, etc). In addition, I improved my PCB design skills, my c and c++ programming ability, and my SMD soldering skills for what that's worth.

In addition to technical experience, this project taught me a lot about managing available resources (specifically I used two 16 bit timers to control 6 different fans), as well as the importance of prototyping, considering my initial circuit layout was incorrect, and I only discovered it after ordering the first round of circuit boards while I was testing the discrete components on their own.

Finally, this fan controller took me longer to get right than any project I've worked on since. Possibly due to inexperience, or possibly due to poor selection of components, I spent close to 4 hours a day on this project for the better part of a Summer (all of my evenings). First on the hardware, then the GUI, and then integrating it all together, I would guess I spent close to 300 hours on this project. I consider the completion of this project to be a matter of personal pride and a great demonstration of my perseverance. I thought I'd give up when I couldn't measure the fan speed accurately, or when the GUI wouldn't communicate with the fan controller, or when Atmel Studio corrupted my project files (without me knowing), but every time I woke up the next morning ready to keep working.

I wish I could provide a more in depth overview of my decision making process, and the process of producing the device, but it's been a few years, and I've honestly forgotten so much.

![Fan Controller image](FanController.jpg)
