# Copyright 2016 Mycroft AI, Inc.
#
# This file is part of Mycroft Core.
#
# Mycroft Core is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# Mycroft Core is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with Mycroft Core.  If not, see <http://www.gnu.org/licenses/>.

# Mycroft libraries
from os.path import dirname

from adapt.intent import IntentBuilder
from mycroft.skills.core import MycroftSkill
from mycroft.util.log import getLogger

import requests


__author__ = 'pipe102030'

LOGGER = getLogger(__name__)

class IotLampSkill(MycroftSkill):


    def __init__(self):
        super(IotLampSkill, self).__init__(name="IotLampSkill")
	self.color_map = {'black': 0, 'white': 1, 'blue': 2, 'green': 3, 'orange': 4, 'red': 5, 'purple': 8, 'yellow': 9, 'pink': 10}	


    def initialize(self):
	self.load_data_files(dirname(__file__))

        lamp_command_intent = IntentBuilder("LampCommandIntent").require("LampKeyword").require("Action").build()
        self.register_intent(lamp_command_intent, self.handle_lamp_command_intent)

	lamp_color_intent = IntentBuilder("LampColorIntent").require("LampColorKeyword").require("ColorName").build()
	self.register_intent(lamp_color_intent, self.handle_lamp_color_intent)

	feeder_intent = IntentBuilder("FishFeederIntent").require("Feeder").build()
	self.register_intent(feeder_intent, self.handle_feeder_intent)

    def handle_lamp_command_intent(self, message):
        action_word = message.data.get("Action")
        device_word = message.data.get("DeviceName")
        LOGGER.info("Command word: " + action_word)
        if action_word == "on" and device_word == "door" :
		    self.speak_dialog("lamp.on")
		    r = requests.get('http://ip_here/lamp?cmd=1')
            
	elif action_word == "off":
		self.speak_dialog("lamp.off")
		r = requests.get('http://ip_here/lamp?cmd=0')
	else:
		self.speak("not sure about that")  	

    def handle_lamp_color_intent(self, message):	
	lamp_color = message.data.get("ColorName")
	LOGGER.info("Lamp Color: " + lamp_color)
	if self.color_map.has_key(lamp_color):
		self.speak_dialog("lamp.color",{"color": lamp_color})
		color_index = self.color_map[lamp_color]
		r = requests.get('http://ip_here/color?color='+str(color_index))
	else:
		self.speak_dialog("lamp.color.error",{"color": lamp_color})

    
    def handle_feeder_intent(self,message):
        self.speak_dialog("feeder")
	r = requests.get('http://ip_here/stepper')	

    def stop(self):
        pass

def create_skill():
    return IotLampSkill()
