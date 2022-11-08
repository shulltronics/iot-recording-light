import socket
import requests
import ipaddress as ipa

socket.timeout(0.02)

print("testing")
LOCAL_HOSTNAME = socket.gethostname()
print("hostname of THIS machine is", LOCAL_HOSTNAME)
# Get the IP address of this machine, as well as the LAN
LOCAL_IP = ipa.ip_address(socket.gethostbyname(LOCAL_HOSTNAME))
print(LOCAL_IP)
print(str(LOCAL_IP) + "/24")
LOCAL_NETWORK = ipa.ip_network(str(LOCAL_IP) + "/24", strict=False)
print("ip address of THIS machine is", LOCAL_IP)
print("LAN network is", LOCAL_NETWORK)

# Scan the LAN for hosts with port 80 open, and then send a GET request to them.
# Search the returned headers for 'Product: recording-light'.
# Return all matches in a list of IP addresses (as strings).
def get_recording_light_ips():
    device_ip = []
    for i, host in enumerate(LOCAL_NETWORK.hosts()):
        host = str(host)
        try:
            s = socket.create_connection((host, 80), timeout=0.06)
            print("bound to port 80 on host {}".format(host))
            s.close()
            # Seems like this timeout needs to be pretty long for this device
            r = requests.get('http://' + host, timeout=1.5)
            # print(r.headers)
            # print(r.headers['Product'])
            if r.headers['Product'] == 'recording-light':
                print("host {} is a recording light!".format(host))
                device_ip.append(host)
            # (hn, _, _) = socket.gethostbyaddr(host)
            # print(hn)
        except KeyError:
            # happens when the returned headers don't have 'Product'
            print("host {} is not a recording light.".format(host))
        except (TimeoutError, ConnectionError, OSError):
            pass
        print("{:3d}".format(i), end='\r')
    return device_ip

devices = get_recording_light_ips()
print("Recording light IP addresses are {}".format(devices))