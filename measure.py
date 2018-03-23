import urllib.request
import json
import time

def parse_rest_reponse(response):
    jsonr = response[:-2]
    jsondata = json.loads(jsonr)
    return jsondata['ugm3']


while True:
    urllib.request.urlopen("http://192.168.1.150/dust") #DHCP address
    time.sleep(1)
    response = urllib.request.urlopen("http://192.168.1.150/ugm3").read()
    val = parse_rest_reponse(response)
    current_time = time.ctime()
    print('Time: ' + current_time + ', sensor measures: ' + str(val) + " ug/m3")