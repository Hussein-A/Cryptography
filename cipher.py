from abc import ABC, abstractmethod
import string

class Cipher(ABC):

    @abstractmethod
    def encrypt(self, plain_text, key):
        pass

    @abstractmethod
    def decrypt(self, cipher_text, key):
        pass

    def cleanText(self, text):
        cleaned_text = text.upper()
        cleaned_text = ''.join(ch for ch in cleaned_text if ch in string.ascii_uppercase)
        return cleaned_text

class Caeser(Cipher):

    def encrypt(self, plain_text, key):
        clean_text = self.cleanText(plain_text)
        cipher_text = ''.join(rotateCharBy(ch) for ch in clean_text)
        print(cipher_text)
            

    def decrypt(self, cipher_text, key):
        pass

    def rotateCharBy(char, rotation):
        assert char in string.ascii_uppercase
        rotation %= 26
        
        charAsInt = ord(char)
        charAsInt = (charAsInt + rotation)%26

        return chr(charAsInt)


        
        
    
