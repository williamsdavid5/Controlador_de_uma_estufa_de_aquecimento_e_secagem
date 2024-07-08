class Btn {
public:
  int *pino;
  bool btnclicado;
  bool btnliberado;
  int ultimoEstadoBtn = LOW;
  unsigned long tempoPrimeiroAcionamento = 0;
  unsigned long tempoDebounce = 50;
  typedef void (*funcao)();

  Btn(int p) {
    btnclicado = false;
    btnliberado = false;
    this->pino = p;
  }

  void clique(funcao f) {
    int leitura = digitalRead(*pino);
    if (leitura != ultimoEstadoBtn) {
      tempoPrimeiroAcionamento = millis();
    }
    if (millis() - tempoPrimeiroAcionamento > tempoDebounce) {
      if (digitalRead(*pino)) {
        btnclicado = true;
        btnliberado = false;
      } else {
        btnliberado = true;
      }

      if (btnclicado && btnliberado) {
        btnclicado = false;
        btnliberado = false;
        f();
      }
    }
    ultimoEstadoBtn = leitura;
  }
};
