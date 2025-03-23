#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <time.h>

// Vetores fornecidos (substitua pelos vetores reais do professor)
int vector1[] = {1, 5, 2, 8, 9, 3, 11, 4, 10, 6, 7, 12, 15, 13, 14, 16}; // Vetor de teste menor
int vector2[] = { /* Vetor de tamanho 2^16, preenchido com valores únicos */ }; // Vetor de teste maior

int main(int argc, char *argv[]) {
  int rank, size, i, local_size, start, end, element_to_find, found_index = -1;
  int *local_vector, *vector;
  double start_time, end_time;
  int vector_size; // Tamanho do vetor para teste

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  // Escolha do vetor e elemento a ser procurado (para testes)
  if (argc > 1 && atoi(argv[1]) == 1) {
      vector = vector1;
      vector_size = sizeof(vector1) / sizeof(vector1[0]);
      element_to_find = 11; // Elemento existente no vector1
  } else {
      // Usar o vetor maior (substitua o vetor2 por um vetor real de 2^16 elementos únicos)
      // Nota importante: alocar memória dinamicamente para vetores grandes
      vector_size = 65536; // 2^16
      vector = (int*) malloc(vector_size * sizeof(int));
      if (vector == NULL) {
          printf("Erro: Falha na alocação de memória para o vetor.\n");
          MPI_Abort(MPI_COMM_WORLD, 1);
      }

      // Inicializa o vetor com valores únicos (exemplo simples - deve ser melhorado para o caso geral)
      for (i = 0; i < vector_size; i++) {
          vector[i] = i + 1; // Exemplo: valores de 1 a 65536
      }

      element_to_find = 32768; // Elemento existente no vector2 (meio do vetor)
  }

  start_time = MPI_Wtime();

  // Distribuição do vetor
  local_size = vector_size / size;
  start = rank * local_size;
  end = (rank == size - 1) ? vector_size : start + local_size; // Ajuste para o último processo

  local_vector = (int*) malloc(local_size * sizeof(int));

  MPI_Scatter(vector, local_size, MPI_INT,
              local_vector, local_size, MPI_INT,
              0, MPI_COMM_WORLD);


  // Busca local
  for (i = 0; i < local_size; i++) {
    if (local_vector[i] == element_to_find) {
      found_index = start + i; // Calcula o índice global
      break;
    }
  }

  // Comunicação dos resultados
  int global_found_index = -1;
  MPI_Reduce(&found_index, &global_found_index, 1, MPI_INT, MPI_MAX, 0, MPI_COMM_WORLD);

  end_time = MPI_Wtime();

  // Impressão do resultado no processo master
  if (rank == 0) {
    if (global_found_index != -1) {
      printf("Elemento %d encontrado na posição %d\n", element_to_find, global_found_index);
    } else {
      printf("Elemento %d não encontrado no vetor.\n", element_to_find);
    }
    printf("Tempo de execução: %f segundos\n", end_time - start_time);
  }

  // Liberação da memória alocada
  free(local_vector);
  if (rank == 0 && vector_size == 65536) {  // Libera somente se alocou (vetor grande)
      free(vector);
  }

  MPI_Finalize();
  return 0;
}
