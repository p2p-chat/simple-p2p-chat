#include <stdio.h>
#include "rsa.h"
#include <string.h>
#include <stdlib.h>


int main(int argc, char **argv)
{
  struct public_key pub[1];
  struct private_key priv[1];
  rsa_gen_keys(pub, priv);

  printf("Private Key:\n Modulus: %lld\n Exponent: %lld\n", (long long)priv->modulus, (long long) priv->exponent);
  printf("Public Key:\n Modulus: %lld\n Exponent: %lld\n", (long long)pub->modulus, (long long) pub->exponent);
  
  char message[] = "111123abc";
  int i;

  printf("Original:\n");
  for(i=0; i < strlen(message); i++){
    printf("%lld\n", (long long)message[i]);
  }

  long long *encrypted = rsa_encrypt(message, sizeof(message), pub);
  if (!encrypted){
    printf("Error in encryption!\n");
    return 1;
  }
  printf("Encrypted:\n");
  for(i=0; i < strlen(message); i++){
    printf("%lld\n", (long long)encrypted[i]);
  }  
  
  char *decrypted = rsa_decrypt(encrypted, 8*sizeof(message), priv);
  if (!decrypted){
    fprintf(stderr, "Error in decryption!\n");
    return 1;
  }
  printf("Decrypted:\n");
  for(i=0; i < strlen(message); i++){
    printf("%lld\n", (long long)decrypted[i]);
  }  
  
  printf("\n");
  free(encrypted);
  free(decrypted);
  return 0;
}
