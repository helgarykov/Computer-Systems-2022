/* To compute the Internet checksum for the data, we need to follow these steps:
 
 Convert the ASCII representation of the string "Networking" to its binary equivalent.
 This will give us a sequence of 10 bytes.

 Divide the data into 16-bit words. If the number of bytes is not a multiple of 16,
 we need to pad the data with zeros until it is.

 Compute the sum of all 16-bit words.

 Invert the sum using one's complement.*/




def compute_checksum(data):
  # Convert ASCII representation to binary
  binary_data = ''.join([format(ord(c), '08b') for c in data])

  # Pad data with zeros until it is a multiple of 16 bits
  while len(binary_data) % 16 != 0:
    binary_data += '0'

  # Split data into 16-bit words
  words = [binary_data[i:i+16] for i in range(0, len(binary_data), 16)]

  # Compute sum of all words
  sum = 0
  for word in words:
    sum += int(word, 2)

  # Invert sum using one's complement
  checksum = ~sum & 0xFFFF

  return checksum

data = "Networking"
checksum = compute_checksum(data)
print(checksum)

/* P4 */

def compute_checksum(data, data_type):
  if data_type == 'numbers':
    # Convert numbers to binary
    binary_data = ''.join([format(n, '04b') for n in data])
  elif data_type == 'uppercase':
    # Convert ASCII representation to binary
    binary_data = ''.join([format(ord(c), '08b') for c in data])
  elif data_type == 'lowercase':
    # Convert ASCII representation to binary
    binary_data = ''.join([format(ord(c), '08b') for c in data])

  # Pad data with zeros until it is a multiple of 16 bits
  while len(binary_data) % 16 != 0:
    binary_data += '0'

  # Split data into 16-bit words
  words = [binary_data[i:i+16] for i in range(0, len(binary_data), 16)]

  # Compute sum of all words
  sum = 0
  for word in words:
    sum += int(word, 2)

  # Invert sum using one's complement
  checksum = ~sum & 0xFFFF

  return checksum

# Test with numbers
data = list(range(1, 11))
checksum = compute_checksum(data, 'numbers')
print(checksum)

# Test with uppercase letters
data = 'BCDEFGHIJK'
checksum = compute_checksum(data, 'uppercase')
print(checksum)

# Test with lowercase letters
data = 'bcdefghijk'
checksum = compute_checksum(data, 'lowercase')
print(checksum)


