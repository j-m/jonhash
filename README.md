# jonhash

Takes raw password for input and outputs a salted hash.
This was made for a university coursework task and has not been tested to ensure security. 
If you spot something that makes it insecure, flag it as an Issue.

Currently converting it to C. Please use the releases tab for the working C++ version.
This does not use a cryptographically secure pseudo random number genertor (CSPRNG) yet, and will have biased output.
Cost, resultant size, Base64 characters, and the initial hash can be easily redefined.

## How does it work?

Please note: currently input can only contain `BASE64` (priorly `CHARACTERS`) characters - this will be improved after the C conversion.

Firstly it generates a random string of `BLOCK_SIZE` (32 by default) characters in Base64. The following characters are used by default:
`ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/`

Then the salt and hash are weaved. 
Say the salt was `ABCDEFGHIJKLMNOPQRSTUVWXYZ012345` and the password was `this is my password`, the weaved result would be: `AtBhCiDsE FmGyH IpJaKsLsMwNoOrPdQRSTUVWXYZ012345`. If the password is longer than `BLOCK_SIZE` then the remainder is appended.

After this, the weave is then padded to a multiple of `BLOCK_SIZE`. The first character (`1`) of the pad is different to the rest (`0`) because if the password were longer than `BLOCK_SIZE` and ended with the same character as the pad, then multiple passwords could have the same hash input (though incredibly unlikely). An additional `BLOCK_SIZE` of `0`'s is appended to the alignment to protect from malicous input. In our case the hash input would be: `AtBhCiDsE FmGyH IpJaKsLsMwNoOrPdQRSTUVWXYZ012345100000000000000000000000000000000000000000000000`.

The initial hash (`H0`) and first block of the hash input (`AtBhCiDsE FmGyH IpJaKsLsMwNoOrP`) are converted to a string of 6-bit binaries. The block binary string is left-shifted for each 1 in the `H0` binary string, plus 1. This result is XORed with the `H0` binary string and converted back to a `BASE64` string. This result is used as the new `H0`, and this repeats for each BLOCK_SIZE chunk in the hash input. 

Finally, the last step is repeated `COST` times (`H0` is **not** reset) to make it a computationally timely operation.
The result is `salt` + `hash`, giving you a `BLOCK_SIZE` * 2 sized string.

<sub><sup>An animation will be made to clarify this, and show an example of the last stages</sup></sub>
