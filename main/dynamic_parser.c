#include "dynamic_parser.h"

#include <string.h>

bool dynamic_parse(char *input, Parsed_Command_t *cmd) {
  char *token;
  uint8_t index = 0;

  memset(cmd, 0, sizeof(Parsed_Command_t));

  token = strtok(input, ":");

  while ((token != NULL) && (index < MAX_TOKENS)) {
    strncpy(cmd->tokens[index], token, MAX_TOKEN_LENGTH - 1);

    cmd->tokens[index][MAX_TOKEN_LENGTH - 1] = '\0';

    index++;

    token = strtok(NULL, ":");
  }

  cmd->token_count = index;

  return (index > 0);
}