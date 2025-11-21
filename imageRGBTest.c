// imageRGBTest - A program that performs some operations on RGB images.
//
// This program is an example use of the imageRGB module,
// a programming project for the course AED, DETI / UA.PT
//
// You may freely use and modify this code, NO WARRANTY, blah blah,
// as long as you give proper credit to the original and subsequent authors.
//
// The AED Team <jmadeira@ua.pt, jmr@ua.pt, ...>
// 2025

#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "error.h"
#include "imageRGB.h"
#include "instrumentation.h"

int main(int argc, char* argv[]) {
  program_name = argv[0];
  if (argc != 1) {
    error(1, 0, "Usage: imageRGBTest");
  }

  ImageInit();

  // Creating and displaying some images

  printf("1) ImageCreate\n");
  Image white_image = ImageCreate(100, 100);
  // ImageRAWPrint(white_image);

  printf("2) ImageCreateChess(black)+ ImageSavePBM\n");
  Image image_chess_1 = ImageCreateChess(150, 120, 30, 0x000000);  // black
  // ImageRAWPrint(image_chess_1);
  ImageSavePBM(image_chess_1, "chess_image_1.pbm");

  printf("3) ImageCreateChess(red) + ImageSavePPM\n");
  Image image_chess_2 = ImageCreateChess(20, 20, 8, 0xff0000);  // red
  ImageRAWPrint(image_chess_2);
  ImageSavePPM(image_chess_2, "chess_image_2.ppm");

  printf("4) ImageCreateChess(all black)\n");
  Image black_image = ImageCreateChess(100, 100, 100, 0x000000);  // all black
  // ImageRAWPrint(black_image);
  ImageSavePBM(black_image, "black_image.pbm");

  printf("5) ImageCopy\n");
  Image copy_image = ImageCopy(image_chess_1);
  // ImageRAWPrint(copy_image);
  if (copy_image != NULL) {
    ImageSavePBM(copy_image, "copy_image.pbm");
  }

  printf("6) ImageLoadPBM\n");
  Image image_1 = ImageLoadPBM("img/feep.pbm");
  ImageRAWPrint(image_1);

  printf("7) ImageLoadPPM\n");
  Image image_2 = ImageLoadPPM("img/feep.ppm");
  ImageRAWPrint(image_2);

  printf("8) ImageCreatePalete\n");
  Image image_3 = ImageCreatePalete(4 * 32, 4 * 32, 4);
  ImageSavePPM(image_3, "palete.ppm");

  // ---------------------------------------------------------
  // NOVO: CODIGO ADICIONADO PARA TESTES DE COMPLEXIDADE
  // ---------------------------------------------------------
  printf("\n============================================\n");
  printf("9) Testes de Complexidade: ImageIsEqual\n");
  printf("============================================\n");
  printf("%-10s %-15s %-15s\n", "Tamanho", "Pior Caso (Ops)", "Melhor Caso (Ops)");
  
  uint32 sizes[] = {100, 200, 500, 1000}; // Tamanhos a testar
  int num_sizes = 4;

  for(int i = 0; i < num_sizes; i++) {
      uint32 sz = sizes[i];
      
      // Preparar imagens para PIOR CASO (Iguais)
      Image imgA = ImageCreate(sz, sz);
      Image imgB = ImageCreate(sz, sz); // Tambem toda branca
      
      InstrReset(); // Reset contador PIXMEM
      ImageIsEqual(imgA, imgB);
      unsigned long pior_caso = InstrCount[0];
      
      // Preparar imagens para MELHOR CASO (Diferentes no inicio)
      // Como imgA e branca, criamos uma preta (xadrez todo preto) para imgB
      ImageDestroy(&imgB);
      imgB = ImageCreateChess(sz, sz, sz, 0x000000); 
      
      InstrReset(); // Reset contador PIXMEM
      ImageIsEqual(imgA, imgB);
      unsigned long melhor_caso = InstrCount[0];
      
      printf("%dx%d      %-15lu %-15lu\n", sz, sz, pior_caso, melhor_caso);
      
      ImageDestroy(&imgA);
      ImageDestroy(&imgB);
  }
  printf("Nota: O Caso Medio estima-se teoricamente como (Pior Caso / 2).\n");
  // ---------------------------------------------------------

  ImageDestroy(&white_image);
  ImageDestroy(&black_image);
  if (copy_image != NULL) {
    ImageDestroy(&copy_image);
  }
  ImageDestroy(&image_chess_1);
  ImageDestroy(&image_chess_2);
  ImageDestroy(&image_1);
  ImageDestroy(&image_2);
  ImageDestroy(&image_3);

  return 0;
}