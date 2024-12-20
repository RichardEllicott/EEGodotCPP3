"""

"""


print("OUTPUT STUFF::")



freeverb_comb_tunings = [1116, 1188, 1277, 1356, 1422, 1491, 1557, 1617]

sample_rate = 44100.0

resample_rate = 48000.0


actual_tunings = [] # as floats, in seconds

for tuning in freeverb_comb_tunings:
    actual_tunings.append(float(tuning) / sample_rate)

print("actual_tunings:", actual_tunings)



for i in range(len(actual_tunings) - 1):

    tuning1 = actual_tunings[i]
    tuning2 = actual_tunings[i+1]

    print(tuning2 - tuning1)




# print("tuning_distance:", tuning_distance)


actual_frequency = []
for tuning in actual_tunings:
    actual_frequency.append(1.0 / tuning)

print("actual_frequency:", actual_frequency)




print("GAPS")
gaps = [
0.0016326530612244886,
0.002018140589569159,
0.0017913832199546488,
0.0014965986394557818,
0.0015646258503401386,
0.0014965986394557818,
0.001360544217687075
]

total = 0.0

for gap in gaps:
    total += gap

average = total / len(gaps)

print("average", average)