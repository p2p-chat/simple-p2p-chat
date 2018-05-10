#include <stdio.h>
#include "rsa.h"
#include <string.h>
#include <stdlib.h>


int main(int argc, char **argv)
{
  rsa_key_t pub[1];
  rsa_key_t priv[1];
  rsa_gen_keys(pub, priv);

  printf("Private Key:\n Modulus: %lld\n Exponent: %lld\n", (long long)priv->modulus, (long long) priv->exponent);
  printf("Public Key:\n Modulus: %lld\n Exponent: %lld\n", (long long)pub->modulus, (long long) pub->exponent);
  
  char message[] = "111123aasdbasdc";
  int i;

  printf("Original:\n");
  for(i=0; i < strlen(message); i++)
  {
    printf("%lld\n", (long long)message[i]);
  }

  long long *message_crypted = NULL;

  if (!rsa_encrypt(message, sizeof(message), &message_crypted, pub))
  {
    printf("Error in encryption!\n");
    return 1;
  }
  printf("Encrypted:\n");
  for(i=0; i < strlen(message); i++)
  {
    printf("%lld\n", (long long)message_crypted[i]);
  }

  char *message_decrypted;
  if (!rsa_decrypt(message_crypted, 8*sizeof(message), &message_decrypted, priv))
  {
    fprintf(stderr, "Error in decryption!\n");
    return 1;
  }
  printf("Decrypted:\n");
  for(i=0; i < strlen(message); i++){
    printf("%lld\n", (long long)message_decrypted[i]);
  }

  // printf("message = %d, message_crypted = %d, message_decrypted = %d\n",
            // strlen(message), strlen((unsigned char *)message_crypted), strlen(message_decrypted));
  free(message_crypted);
  free(message_decrypted);
  return 0;
}
