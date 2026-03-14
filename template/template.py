__HEADER_PLACEHOLDER__

class Lexer:
    """
    A generated lexer class using Deterministic Finite Automata (DFA).
    The transition and acceptance tables are injected during generation.
    """
    
__TABLES_PLACEHOLDER__

    def __init__(self, input_stream=sys.stdin):
        """
        Initializes the lexer.
        :param input_stream: The file-like object to read from (defaults to stdin).
        """
        self.input_stream = input_stream
        self.yytext = ""
        self.yyleng = 0
        self.yylineno = 1
        self.yy_start = 0
        self.yy_at_bol = True
        self.pushback_buffer = []  # Stack for unreading characters

    def BEGIN(self, state):
        self.yy_start = state

    def read_char(self):
        """Reads one character from the input stream."""
        if self.pushback_buffer:
            return self.pushback_buffer.pop()
        char = self.input_stream.read(1)
        return char if char else None

    def unread_char(self, char):
        """Puts back one character for later reading."""
        if char is not None:
            self.pushback_buffer.append(char)

    def input(self):
        c = self.read_char()
        if c == '\n':
            self.yylineno += 1
            self.yy_at_bol = True
        elif c is not None:
            self.yy_at_bol = False
        return c

    def unput(self, c):
        if c == '\n':
            self.yylineno -= 1
        self.unread_char(c)

    def yyless(self, n):
        for i in range(len(self.yytext) - 1, n - 1, -1):
            self.unput(self.yytext[i])
        self.yytext = self.yytext[:n]
        self.yyleng = n

    def _do_eof_action(self, cond_idx):
        return None # Placeholder, overridden by generator

    def yylex(self):
        """
        The main lexer driver function.
        It runs the DFA and returns a matched token ID or 0 for EOF.
        """
        
        while True:
            current_state = self.YY_START_STATE_IDX[self.yy_start]
            last_accepting_state = -1
            
            buffer = []
            last_accepting_idx = -1
            
            # Read first char
            c = self.read_char()
            if c is None:
                ret = self._do_eof_action(self.yy_start)
                if ret is not None: return ret
                return 0  # EOF
            
            # Check for BOL anchor (256)
            if self.yy_at_bol:
                anchor_state = self.YY_NXT[current_state][256]
                if anchor_state != -1:
                    current_state = anchor_state
                    if self.YY_ACCEPT[current_state] != -1:
                        last_accepting_state = current_state
                        last_accepting_idx = 0

            buffer.append(c)
            
            # Initial transition
            char_code = ord(c) if 0 <= ord(c) < 256 else 0
            next_state = self.YY_NXT[current_state][char_code]
            
            while next_state != -1:
                current_state = next_state
                
                if self.YY_ACCEPT[current_state] != -1:
                    last_accepting_state = current_state
                    last_accepting_idx = len(buffer)
                
                c = self.read_char()
                if c is None:
                    # Check for EOL anchor (257) on EOF
                    eol_state = self.YY_NXT[current_state][257]
                    if eol_state != -1 and self.YY_ACCEPT[eol_state] != -1:
                        last_accepting_state = eol_state
                        last_accepting_idx = len(buffer)
                    break
                
                if c == '\n':
                    # Check for EOL anchor (257)
                    eol_state = self.YY_NXT[current_state][257]
                    if eol_state != -1 and self.YY_ACCEPT[eol_state] != -1:
                        last_accepting_state = eol_state
                        last_accepting_idx = len(buffer)

                buffer.append(c)
                char_code = ord(c) if 0 <= ord(c) < 256 else 0
                next_state = self.YY_NXT[current_state][char_code]
            
            # No more transitions
            if last_accepting_state != -1:
                # Match found! Backtrack.
                while len(buffer) > last_accepting_idx:
                    self.unread_char(buffer.pop())
                
                self.yytext = "".join(buffer)
                self.yyleng = len(buffer)
                
                # Update BOL and lineno
                for char in self.yytext:
                    if char == '\n':
                        self.yylineno += 1
                
                if self.yytext:
                    self.yy_at_bol = (self.yytext[-1] == '\n')

                match_id = self.YY_ACCEPT[last_accepting_state]
                ret = self._do_action(match_id)
                if ret is not None: return ret

            else:
                # No match. Echo first char and unput everything else.
                sys.stdout.write(buffer[0])
                sys.stdout.flush()
                if buffer[0] == '\n':
                    self.yylineno += 1
                    self.yy_at_bol = True
                else:
                    self.yy_at_bol = False
                while len(buffer) > 1:
                    self.unread_char(buffer.pop())

__YYLEX_BODY_PLACEHOLDER__

__USER_CODE_PLACEHOLDER__

if __name__ == '__main__':
    
    if len(sys.argv) > 1:
        try:
            with open(sys.argv[1], 'r') as f:
                lexer = Lexer(f)
                lexer.yylex()
        except FileNotFoundError:
            print(f"Error: Input file '{sys.argv[1]}' not found.", file=sys.stderr)
            sys.exit(1)
    else:
        # Read from standard input (stdin)
        lexer = Lexer(sys.stdin)
        lexer.yylex()
