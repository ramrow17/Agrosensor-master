# Agrosensor

This repository is to store all the code in one place for our UNT Senior Design 2020 Agrosensor Automated Plant Monitoring System project. We have seperated the files into server_files (files that run the webserver) arduino_files, and so on.


# Objective

Agrosensor is an automated plant monitoring system that monitors your plants health and sends alerts, via our app, to the user about when to water their plant, if their plants are healthy or not, and all the plant information they need.
Through our app, the user will be able to look at the history of their plant health, take pictures of their plant through our simple camera sensor embedded in the plant soil (for times when they're away and want to check up on their plant's health), or whatever they need to do in order to make sure their plant is healthy. With our product the days of inadvertently killing your house plants, or on a larger scale, your crops, are a thing of the past.


# How To Implement Agrosensor

We are providing this guide so that you will be easily able to implement Agrosensor yourself. The equipment (sensors, arduino boards, etc.) are what we used. If you want to use you own sensors that's fine, but the arduino code might not work if you use a board other than the Arduino Uno Wifi REV 2. We have broken down the implementation process in the following steps:

### Implementing the web server

Implementing the web server will be easy. Implement Nginx on an Ubuntu web server and install PHP and MySQL. If you don't know how to do this, tutorials on how to do so are easy to find and follow. If this is the first time you're doing something like this we recommend using Digital Ocean.

Once you're server is set up, simply add the files contained in server_files to the /var/www/html directory on your server. Again, make sure your server has php, and mysql installed. 

### Implementing the Arduino and Sensor Code

This section highlights how to implement the Arduino and Sensor code

### Implementing the phone app code

This section explains how to implement the app code


## Agrosensor site

If you want to visit the site click [here](http://157.245.185.143/). We have only allowed you to interact with a tiny functionality of our project. Our server is the brain of the whole operation. 



