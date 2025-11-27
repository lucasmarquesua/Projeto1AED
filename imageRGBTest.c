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

  // ---------------------------------------------------------
  // TESTES FUNCIONAIS: Tranformacoes, Region Growing e Segmentacao
  // ---------------------------------------------------------

  printf("\n============================================\n");
  printf("10) Testes de Transformacoes Geometricas\n");
  printf("============================================\n");
  
  // Criar uma imagem não-quadrada (ex: 30x20) para testar rotação de dimensões
  Image img_base = ImageCreateChess(30, 20, 5, 0x0000FF); // Azul
  ImageSavePPM(img_base, "test_base.ppm");
  
  // Teste Rotate90
  Image img_rot90 = ImageRotate90CW(img_base);
  if (img_rot90) {
      printf("Rotate90: Original %dx%d -> Rodada %dx%d ", 
             ImageWidth(img_base), ImageHeight(img_base), 
             ImageWidth(img_rot90), ImageHeight(img_rot90));
      if (ImageWidth(img_rot90) == ImageHeight(img_base) && 
          ImageHeight(img_rot90) == ImageWidth(img_base)) {
          printf("[OK Dimensoes]\n");
          ImageSavePPM(img_rot90, "test_rot90.ppm");
      } else {
          printf("[ERRO Dimensoes]\n");
      }
      ImageDestroy(&img_rot90);
  }

  // Teste Rotate180
  Image img_rot180 = ImageRotate180CW(img_base);
  if (img_rot180) {
      printf("Rotate180: %dx%d -> %dx%d ", 
             ImageWidth(img_base), ImageHeight(img_base),
             ImageWidth(img_rot180), ImageHeight(img_rot180));
      if (ImageWidth(img_rot180) == ImageWidth(img_base) && 
          ImageHeight(img_rot180) == ImageHeight(img_base)) {
          printf("[OK Dimensoes]\n");
          ImageSavePPM(img_rot180, "test_rot180.ppm");
      } else {
          printf("[ERRO Dimensoes]\n");
      }
      ImageDestroy(&img_rot180);
  }
  ImageDestroy(&img_base);


  printf("\n============================================\n");
  printf("11) Testes de Region Growing (Flood Fill)\n");
  printf("============================================\n");

  Image img_fill = ImageCreateChess(100, 100, 25, 0x000000); 
  
  // CORREÇÃO: Usamos o índice 1 (Preto) que já existe na LUT.
  // Não podemos chamar LUTAllocColor pois é privada (static).
  uint16 cor_teste = 1; 

  // Teste RECURSIVO
  printf("a) Recursivo: ");
  Image img_rec = ImageCopy(img_fill);
  int count_rec = ImageRegionFillingRecursive(img_rec, 0, 25, cor_teste);
  printf("Pintou %d pixeis. ", count_rec);
  ImageSavePPM(img_rec, "test_fill_recursive.ppm");
  ImageDestroy(&img_rec);
  printf("Verificar 'test_fill_recursive.ppm'\n");

  // Teste STACK
  printf("b) Stack:     ");
  Image img_stack = ImageCopy(img_fill);
  int count_stack = ImageRegionFillingWithSTACK(img_stack, 0, 25, cor_teste);
  printf("Pintou %d pixeis. ", count_stack);
  if (count_stack == count_rec) printf("[OK Contagem igual]");
  else printf("[ERRO Contagem diferente]");
  printf("\n");
  ImageSavePPM(img_stack, "test_fill_stack.ppm");
  ImageDestroy(&img_stack);

  // Teste QUEUE
  printf("c) Queue:     ");
  Image img_queue = ImageCopy(img_fill);
  int count_queue = ImageRegionFillingWithQUEUE(img_queue, 0, 25, cor_teste);
  printf("Pintou %d pixeis. ", count_queue);
  if (count_queue == count_rec) printf("[OK Contagem igual]");
  else printf("[ERRO Contagem diferente]");
  printf("\n");
  ImageSavePPM(img_queue, "test_fill_queue.ppm");
  ImageDestroy(&img_queue);
  
  ImageDestroy(&img_fill);


  printf("\n============================================\n");
  printf("12) Testes de Segmentacao\n");
  printf("============================================\n");
  
  // Criar uma imagem com várias regiões isoladas
  Image img_seg = ImageCreateChess(60, 60, 10, 0x000000); 
  
  printf("A segmentar imagem Xadrez 60x60 (quadrados de 10px)...\n");
  
  // Usamos a função recursiva como motor
  int num_regions = ImageSegmentation(img_seg, ImageRegionFillingRecursive);
  
  printf("Numero de regioes encontradas: %d\n", num_regions);
  
  ImageSavePPM(img_seg, "test_segmentation.ppm");
  printf("Resultado guardado em 'test_segmentation.ppm'. Abra para ver as cores aleatorias.\n");
  
  ImageDestroy(&img_seg);

  return 0;
}