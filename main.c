#include <raylib.h>
#include <stdlib.h>
#include <stdio.h>
#define framesfundoMAX 25
#define disparosmax 100    

typedef enum TelaGame   // ESSE enum SUBSTITUI 0, 1, 2 E 3 PARA LOGO, MENU, GAMEPLAY E FIM RESPECTIVAMENTE.
{
    LOGO = 0,
    MENU,
    GAMEPLAY,
    DIFICULDADE,
    FIM
} TelaGame;

int verificavida = 3;
int i1 = 60;
int i2 = 75;
int i3 = 90;
int i4 = 100;
int aleatorio;
int score = 0;
char score_c[1000];
int vidas = 3;
int jogando = 0;
int contnum = 0;
int conttam = 0;
int conttempovida = 0;
int contcolisao = 0;
int dificuldade = 0;

bool isMusicOver(Music musica){ //checa se uma música chegou ao fim
    if((GetMusicTimePlayed(musica)/GetMusicTimeLength(musica)) >= 1) return true;
    if((GetMusicTimePlayed(musica)/GetMusicTimeLength(musica)) <= 0) return true;
    else return false;
}

void loopMusic(Music musica){ //torna uma música um loop
    if(isMusicOver(musica)) UpdateMusicStream(musica);
}

void gameplayMusic(Music musica, int volume){ //simplifica a troca de temas
    SetMusicVolume(musica, volume);
    UpdateMusicStream(musica);
    loopMusic(musica);
}

bool click(Rectangle rec){ //checa clique do mouse no botao
    if(CheckCollisionPointRec(GetMousePosition(), rec) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) return true;
    else return false;
}

int main()
{
    //inicialização
    int larguraTela = 450, alturaTela = 650, alturaBotao = 35, larguraBotao = 150;
    InitWindow(larguraTela, alturaTela, "ACM > Elon Musk");
    SetWindowState(FLAG_VSYNC_HINT);
    SetTargetFPS(120);
    SetExitKey(KEY_NULL);
    InitAudioDevice();
    SetMasterVolume(0.07);
    
    
    
    bool pausado = false;
    float xNave = 215, yNave = 500;
    int i = 0, flag = 0;
    int contadorframes = 0, framesdisparo = 60;
    int framesfundo = 0;
    unsigned int dataOffset = 0;
    int dificuldade = 5;
    int min = 0;
    int max = 450;
    int x;
    int y;

    
    
    TelaGame telaAtual = LOGO;   //Tela atual do jogo inicializada na imagem de NOME DO JOGO. 
    
    //definição dos projéteis
    typedef struct{
        float posx;
        float posy;
        float speed;
        int scale;
        Color cor;
        bool ativo;
    } disparo;
    
    disparo disparos[disparosmax];
    for(i=0;i<disparosmax;i++){
        disparos[i].posx = xNave;
        disparos[i].posy = yNave;
        disparos[i].speed = 3;
        disparos[i].scale = 1;
        disparos[i].cor = WHITE;
        disparos[i].ativo = false;
    }
    
    //informações disparos do inimigo
    typedef struct{
        float posxproj;
        float posyproj;
        int ativo;
        int perigo;
    } infoprojeteis;
    
    //informações do inimigo
    typedef struct{
        float xinmg;
        float yinmg;
        int ativo;
        int tipo;
        int vida;
        infoprojeteis projeteis[10];
        float velocidade;
        float dano;
        float pontuacao;
        float velproj;
    } dados;
    
    float velocidade[4] = {1,2,1,1};
    float dano[4] = {1,1,1,2};
    float pontuacao[4] = {10,20,20,30};
    float velproj[4] = {2,2,3,2};
    
    dados inimigos[10];
    
    for (x = 0; x < dificuldade; x++){
        for (y = 0; y < 10; y++){
            inimigos[x].projeteis[y].ativo = 0;
        }
    }
      
    //6upload das texturas
    
    Image fundoanim = LoadImageAnim("assets/fundo/fundoanimi.gif", &framesfundo);
    Texture2D fundoanimi = LoadTextureFromImage(fundoanim);
    
    
    Texture2D fundoLOGO = LoadTexture("assets/fundo/fundoOFICIAL.png"); //Fundo menu 'logo'
    Texture2D texNave = LoadTexture("assets/Ships/ship_0002.png"); //nave jogador
    Texture2D texDisparo = LoadTexture("assets/Tiles/tile_0002.png"); //disparo jogador
    Texture2D explos1 = LoadTexture("assets/Tiles/tile_0004.png"); //explosão inimigo 1
    Texture2D explos2 = LoadTexture("assets/Tiles/tile_0005.png"); //explosão inimigo 2 
    
    Image ImgInmg1 = LoadImage("assets/Ships/ship_0017.png");
    Image ImgInmg2 = LoadImage("assets/Ships/ship_0023.png");
    Image ImgInmg3 = LoadImage("assets/Ships/ship_0014.png");
    Image ImgInmg4 = LoadImage("assets/Ships/ship_0012.png");
    Image ImgProjInmg = LoadImage("assets/Tiles/tile_0002.png");
    ImageFlipVertical(&ImgInmg1);
    ImageFlipVertical(&ImgInmg2);
    ImageFlipVertical(&ImgInmg3);
    ImageFlipVertical(&ImgInmg4);
    ImageFlipVertical(&ImgProjInmg);
    Texture2D texInmg1 = LoadTextureFromImage(ImgInmg1);
    Texture2D texInmg2 = LoadTextureFromImage(ImgInmg2);
    Texture2D texInmg3 = LoadTextureFromImage(ImgInmg3);
    Texture2D texInmg4 = LoadTextureFromImage(ImgInmg4);
    Texture2D TexProjInmg = LoadTextureFromImage(ImgProjInmg);
    
    Image coracao = LoadImage("assets/novos/coracao2.png"); //corações
    ImageResize(&coracao, 20, 20); // Redimensiona a imagem
    Texture2D minhaTextura = LoadTextureFromImage(coracao); // Converte a imagem em uma textura
    UnloadImage(coracao); // Descarrega a imagem original
    
    //upload SFX e Música
    Sound disparoSFX = LoadSound("assets/SFX/Lasers/Laser.wav"); //disparos jogador
    Sound danoSFX = LoadSound("assets/SFX/Hit/Hit_3.wav"); //dano no jogador
    Sound hitSFX = LoadSound("assets/SFX/Hit/Hit.wav"); //hit em inimigos
    Sound killSFX = LoadSound("assets/SFX/Explosions/Explosion.wav"); //morte inimigos
    Sound deathSFX = LoadSound("assets/SFX/Explosions/Explosion_2.wav");//morte jogador
    
    Music temaMenu = LoadMusicStream("assets/SFX/Lightyear_City.mp3");
    Music tema1 = LoadMusicStream("assets/SFX/Pink_Bloom.mp3");
    Music tema2 = LoadMusicStream("assets/SFX/The_Hidden_One.mp3");
    
    
    //MENU
    Vector2 posicaoMouse = {0.0f, 0.0f}; // CRIANDO A VARIAVEL PARA O MOUSE
    
    //BOTÕES MENU
    // botão para a opção play/jogar
    Rectangle areaBotaoPLAY = {larguraTela/2 - larguraBotao/2 , alturaTela/2 - alturaBotao/2 - 80, larguraBotao, alturaBotao}; // Area do botão jogar
    // botão para opção DIFICULDADE
    Rectangle areaBotaoDIFICULDADE = {larguraTela/2 - larguraBotao/2 , alturaTela/2 - alturaBotao/2, larguraBotao, alturaBotao}; //Area do botão dificuldade
    //botão para a opção SAIR
    Rectangle areaBotaoSAIR = {larguraTela/2 - larguraBotao/2, alturaTela/2 - alturaBotao/2 + 80, larguraBotao, alturaBotao}; // Area botão SAIR

    // botão para a opção play/jogar
    Rectangle areaBotaoFACIL = {larguraTela/2 - larguraBotao/2 , alturaTela/2 - alturaBotao/2 - 80, larguraBotao, alturaBotao}; // Area do botão jogar
    // botão para opção DIFICULDADE
    Rectangle areaBotaoMEDIO = {larguraTela/2 - larguraBotao/2 , alturaTela/2 - alturaBotao/2, larguraBotao, alturaBotao}; //Area do botão dificuldade
    //botão para a opção SAIR
    Rectangle areaBotaoDIFICIL = {larguraTela/2 - larguraBotao/2, alturaTela/2 - alturaBotao/2 + 80, larguraBotao, alturaBotao}; // Area botão SAIR
    
    //músicas
    PlayMusicStream(temaMenu);
        SeekMusicStream(temaMenu, 15);
    PlayMusicStream(tema1);
    PlayMusicStream(tema2);
    
    //loop do jogo
    while(!WindowShouldClose()){
        
        posicaoMouse = GetMousePosition();  // RECEBENDO A POSIÇÃO DO MOUSE NA TELA
        

        switch (telaAtual)
        {
        case LOGO:
            UpdateMusicStream(temaMenu);
            loopMusic(temaMenu);
            
            if(IsKeyPressed(KEY_SPACE)){
                telaAtual = MENU;
            }
            break;
        
        case MENU:
        
            //reinicia os dados do jogo e gera inimigos
            vidas = verificavida;
            xNave = 215;
            yNave = 500;
            score = 0;
            for (x = 0; x < 5; x++){
                inimigos[x].xinmg = GetRandomValue(min, max);
                inimigos[x].yinmg = -30;
                inimigos[x].ativo = 0;
                inimigos[x].ativo = 0;
                jogando = 0;
                for (y = 0; y < 10; y++){
                    inimigos[x].projeteis[y].ativo = 0;
                }
            }
            
            //toca música
            UpdateMusicStream(temaMenu);
            loopMusic(temaMenu);
            
            //click nos botões
            if(click(areaBotaoPLAY)){
                telaAtual = GAMEPLAY;
            }
            else if(click(areaBotaoDIFICULDADE)){
                telaAtual = DIFICULDADE;
            }
            else if(click(areaBotaoSAIR)){
                CloseAudioDevice();
                CloseWindow();
            }
            break;
        
        case DIFICULDADE:
        
            //toca música
            UpdateMusicStream(temaMenu);
            loopMusic(temaMenu);
            

             if(click(areaBotaoFACIL)){
                // CÓDIGO AQUI PARA A DIFICULDADE SER SETADA PARA FÁCIL AO CLICAR NO BOTÃO FÁCIL
                dificuldade = 5;
                verificavida = 3;
                i1 = 60;
                i2 = 75;
                i3 = 90;
                i4 = 100;
                telaAtual = MENU;
            }
            else if(click(areaBotaoMEDIO)){
                // CÓDIGO AQUI PARA A DIFICULDADE SER SETADA PARA FÁCIL AO CLICAR NO BOTÃO MEDIO
                dificuldade = 7;
                verificavida = 3;
                i1 = 30;
                i2 = 55;
                i3 = 80;
                i4 = 100;
                telaAtual = MENU;
            }
            else if(click(areaBotaoDIFICIL)){
                // CÓDIGO AQUI PARA A DIFICULDADE SER SETADA PARA FÁCIL AO CLICAR NO BOTÃO DIFICIL
                dificuldade = 9;
                verificavida = 1;
                i1 = 10;
                i2 = 40;
                i3 = 70;
                i4 = 100;
                telaAtual = MENU;
            }
            if(IsKeyPressed(KEY_SPACE)){
                telaAtual = MENU;
            }
            break;
            
        case GAMEPLAY:
            framesfundo++;
            if(framesfundo >= framesfundoMAX) framesfundo = 0;
            dataOffset = fundoanimi.width*fundoanimi.height*4*framesfundo;
            if(!pausado) UpdateTexture(fundoanimi, (unsigned char *)fundoanim.data + dataOffset);
        
            //volume menor ao pausar
            dificuldade < 9 ? gameplayMusic(tema1, 0.3) : gameplayMusic(tema2, 0.3); 
            
            //condição p/ morte
            if(vidas <= 0){
                telaAtual = FIM;
            }
            //tecla de pausa
            if(IsKeyPressed(KEY_ESCAPE)) pausado = !pausado;
            
            //libera os inimigos
            if (jogando == 0){
                inimigos[0].ativo = 1;
                aleatorio = GetRandomValue (0,100);
                if (aleatorio <= i1){
                    inimigos[0].tipo = 0;
                } else if (aleatorio <= i2 && aleatorio >i1){
                    inimigos[0].tipo = 1;
                } else if (aleatorio <= i3 && aleatorio >i2){
                    inimigos[0].tipo = 2;
                } else if (aleatorio <= i4 && aleatorio > i3){
                    inimigos[0].tipo = 3;
                }
                inimigos[0].velocidade = velocidade[inimigos[0].tipo];
                inimigos[0].dano = dano[inimigos[0].tipo];
                inimigos[0].pontuacao = pontuacao[inimigos[0].tipo];
                inimigos[0].velproj = velproj[inimigos[0].tipo];
                jogando = 1;
            }
            
            //lógica do jogo
            if(!pausado){
                
                
                //loop da música
                dificuldade < 9 ? gameplayMusic(tema1, 1) : gameplayMusic(tema2, 1);
                
                //contadores atualizam a cada frame
                contadorframes++;
                framesdisparo++;
                conttam++;
                
                //atualização dos inimigos no mapa
                for (x = 0; x < dificuldade; x++){
                    if (inimigos[x].yinmg > 0 && inimigos[x].yinmg < 650){
                        if (inimigos[x].yinmg == 1){
                            inimigos[x].projeteis[0].posyproj = inimigos[x].yinmg + 30;
                            inimigos[x].projeteis[0].posxproj = inimigos[x].xinmg + 8;
                        }
                        
                    }
                    if(inimigos[x].ativo == 1 && inimigos[x].yinmg != 650){
                        if (inimigos[x].velocidade == 1){
                            inimigos[x].yinmg++;
                        } else if (inimigos[x].velocidade == 2){
                            inimigos[x].yinmg++;
                            inimigos[x].yinmg++;
                        }
                    }
                    if (conttam == 150 && contnum < 4){
                        inimigos[contnum + 1].ativo = 1;
                        aleatorio = GetRandomValue(0,100);
                        if (aleatorio <= i1){
                            inimigos[contnum + 1].tipo = 0;
                        } else if (aleatorio <= i2 && aleatorio > i1){
                            inimigos[contnum + 1].tipo = 1;
                        } else if (aleatorio <= i3 && aleatorio > i2){
                            inimigos[contnum + 1].tipo = 2;
                        } else if (aleatorio <= i4 && aleatorio > i3){
                            inimigos[contnum + 1].tipo = 3;
                        }
                        inimigos[contnum + 1].velocidade = velocidade[inimigos[contnum + 1].tipo];
                        inimigos[contnum + 1].dano = dano[inimigos[contnum + 1].tipo];
                        inimigos[contnum + 1].pontuacao = pontuacao[inimigos[contnum + 1].tipo];
                        inimigos[contnum + 1].velproj = velproj[inimigos[contnum + 1].tipo];
                        conttam = 0;
                        contnum++;
                    }
                    else if (conttam == 150 && contnum == 4){
                        inimigos[0].ativo = 1;
                        aleatorio = GetRandomValue(0,100);
                        if (aleatorio <= i1){
                            inimigos[0].tipo = 0;
                        } else if (aleatorio <= i2 && aleatorio >i1){
                            inimigos[0].tipo = 1;
                        } else if (aleatorio <= i3 && aleatorio >i2){
                            inimigos[0].tipo = 2;
                        } else if (aleatorio <= i4 && aleatorio > i3){
                            inimigos[0].tipo = 3;
                        }
                        inimigos[0].velocidade = velocidade[inimigos[0].tipo];
                        inimigos[0].dano = dano[inimigos[0].tipo];
                        inimigos[0].pontuacao = pontuacao[inimigos[0].tipo];
                        inimigos[0].velproj = velproj[inimigos[0].tipo];
                        conttam = 0;
                        contnum = 0;
                    }
                    if (inimigos[x].yinmg == 650 || inimigos[x].ativo == 0){
                        inimigos[x].ativo = 0;
                        inimigos[x].yinmg = -30;
                        inimigos[x].xinmg = GetRandomValue(min, max);
                    }
                    
                    //lógica projéteis inimigos
                    if (inimigos[x].ativo == 1){
                        inimigos[x].projeteis[0].ativo = 1;
                    }
                    if (inimigos[x].tipo == 1){
                        inimigos[x].projeteis[0].ativo = 0;
                    }
                    for (y = 0; y < 10; y++){
                        if ((inimigos[x].projeteis[y].posyproj == inimigos[x].yinmg + 150 &&inimigos[x].tipo != 2) || (inimigos[x].projeteis[y].posyproj == inimigos[x].yinmg + 163 &&inimigos[x].tipo == 2) || (inimigos[x].projeteis[y].posyproj == inimigos[x].yinmg + 162 &&inimigos[x].tipo == 2)){
                            if (inimigos[x].projeteis[y].ativo == 1){
                                if (x < 9){
                                    inimigos[x].projeteis[y + 1].ativo = 1;
                                    if (inimigos[x].projeteis[y + 1].ativo = 1){
                                        inimigos[x].projeteis[y + 1].posyproj = inimigos[x].yinmg + 30;
                                        inimigos[x].projeteis[y + 1].posxproj = inimigos[x].xinmg + 8;
                                    }
                                } else if (x == 9){
                                    inimigos[x].projeteis[0].ativo = 1;
                                    if (inimigos[x].projeteis[0].ativo = 1){
                                        inimigos[x].projeteis[0].posyproj = inimigos[x].yinmg + 30;
                                        inimigos[x].projeteis[0].posxproj = inimigos[x].xinmg + 8;
                                    }
                                }
                            }
                        }
                        if (inimigos[x].projeteis[y].ativo == 1){
                            if ((inimigos[x].yinmg > 1 && inimigos[x].ativo == 1) || inimigos[x].ativo == 0){
                                if (inimigos[x].velproj == 2){
                                    inimigos[x].projeteis[y].posyproj++;
                                    inimigos[x].projeteis[y].posyproj++;
                                } else if (inimigos[x].velproj == 3){
                                    inimigos[x].projeteis[y].posyproj++;
                                    inimigos[x].projeteis[y].posyproj++;
                                    inimigos[x].projeteis[y].posyproj++;
                                }
                            }

                        } else if (inimigos[x].projeteis[y].ativo == 0){
                            inimigos[x].projeteis[y].posyproj = -20;
                            inimigos[x].projeteis[y].posxproj = -100;
                        }
                    }
                }
                
                
                //movimentação jogador
                if(IsKeyDown(KEY_D)&&xNave<415) xNave += 2;
                if(IsKeyDown(KEY_A)&&xNave>3) xNave -= 2;
                if(IsKeyDown(KEY_W)&&yNave>150) yNave -= 2;
                if(IsKeyDown(KEY_S)&&yNave<615) yNave += 2;
                
                
                //atualiza posição inicial do disparo
                for(i=0;i<disparosmax;i++){
                    if(disparos[i].ativo==false){
                        disparos[i].posx = xNave+9;
                        disparos[i].posy = yNave-9;
                    }
                }
                
                //comando de disparo
                if(IsKeyPressed(KEY_J)&&framesdisparo>29){
                    
                    framesdisparo = 0; //delay de 0.5seg por disparo
                    PlaySound(disparoSFX);
                    
                    //"ativa" um disparo por vez
                    for(i=0;i<disparosmax&&flag==0;i++){
                        if(disparos[i].ativo==false){
                            disparos[i].ativo = true;
                            flag = 1;
                            }
                    }
                    flag = 0;
                }
                
            }
            
        
            break;
        
        case FIM:           
            if(IsKeyPressed(KEY_SPACE)){
                telaAtual = MENU;
            }
            break;

        default:
            break;
        }
        
        
        //desenho do jogo
        BeginDrawing();
            
            switch (telaAtual){
                
                case LOGO:
                    ClearBackground(BLACK);
                    DrawTexture(fundoLOGO, larguraTela/2 - fundoLOGO.width/2, alturaTela/2 - fundoLOGO.height/2 , WHITE);

                    DrawText("SPACEX", larguraTela/2 - 110, alturaTela /2 - 200, 50, WHITE);
                    DrawText("Jornada para Marte", larguraTela/2 - 160, alturaTela /2 - 160, 30, WHITE);
                    DrawText("[PRESSIONE A TECLA ESPAÇO]", 65, alturaTela - 30, 20, WHITE);
                    break;
            
                case MENU:
                    ClearBackground(BLACK);
                    DrawText("MENU PRINCIPAL", larguraTela/2 - larguraBotao/2 - 40, alturaTela/2 - alturaBotao/2 - 150, 30, GREEN);

                    //criando botões
                    DrawRectangle(larguraTela/2 - larguraBotao/2 , alturaTela/2 - alturaBotao/2 - 80, larguraBotao, alturaBotao, GREEN);
                    DrawText("PLAY",larguraTela/2 - larguraBotao/2, alturaTela/2 + 10 - alturaBotao/2 - 80, 20, WHITE);

                    DrawRectangle(larguraTela/2 - larguraBotao/2 , alturaTela/2 - alturaBotao/2, larguraBotao, alturaBotao, GREEN);
                    DrawText("DIFICULDADE",larguraTela/2 - larguraBotao/2, alturaTela/2 + 8 - alturaBotao/2 , 20, WHITE);

                    DrawRectangle(larguraTela/2 - larguraBotao/2, alturaTela/2 - alturaBotao/2 + 80, larguraBotao, alturaBotao, GREEN);
                    DrawText("SAIR",larguraTela/2 - larguraBotao/2 , alturaTela/2 - alturaBotao/2 + 85, 20, WHITE);

                    break;

                case DIFICULDADE:
                    ClearBackground(BLACK);
                    DrawText("PRESSIONE ESPAÇO PARA VOLTAR AO MENU", 10, 10, 15, GREEN);
                    DrawText("SELECIONE A DIFICULDADE", larguraTela/2 - larguraBotao/2 - 90, alturaTela/2 - alturaBotao/2 - 150, 25, GREEN);

                    //criando botões
                    DrawRectangle(larguraTela/2 - larguraBotao/2 , alturaTela/2 - alturaBotao/2 - 80, larguraBotao, alturaBotao, GREEN);
                    DrawText("FACIL",larguraTela/2 - larguraBotao/2, alturaTela/2 + 10 - alturaBotao/2 - 80, 20, WHITE);

                    DrawRectangle(larguraTela/2 - larguraBotao/2 , alturaTela/2 - alturaBotao/2, larguraBotao, alturaBotao, GREEN);
                    DrawText("MEDIO",larguraTela/2 - larguraBotao/2, alturaTela/2 + 8 - alturaBotao/2 , 20, WHITE);

                    DrawRectangle(larguraTela/2 - larguraBotao/2, alturaTela/2 - alturaBotao/2 + 80, larguraBotao, alturaBotao, GREEN);
                    DrawText("DIFICIL",larguraTela/2 - larguraBotao/2 , alturaTela/2 - alturaBotao/2 + 85, 20, WHITE);
                    break;
                
                case GAMEPLAY:
                    //\DrawTextureEx(fundoanimi, Vector2 position, float rotation, float scale, WHITE);
                    DrawTexture(fundoanimi,(float) 450/2 - larguraTela/2,(float) 650/2 - alturaTela/2, WHITE);
                    ClearBackground(BLACK);
                    
                    DrawTexture(texNave, xNave, yNave, WHITE);
                    
                    //menu de pausa
                    if(pausado){
                        DrawText("PAUSADO", 120, 120, 40, GREEN);
                        DrawText("[Esc] para continuar", 160, 160, 10, GREEN);
                        
                        Rectangle menuPausa = {larguraTela - 90, alturaTela - 120, 100, 45};
                        DrawRectangleRec(menuPausa, GREEN);
                        DrawText("MENU", larguraTela-80, alturaTela-107, 25, WHITE);
                        
                        if(click(menuPausa)){
                            telaAtual = MENU;
                            pausado = !pausado;
                        }
                    }
                    //menu tutorial temp
                    if(contadorframes<360) DrawText("[Esc] para pausar\nUse [WASD] para se mover\n[J] para atirar", 10, 10, 20, WHITE);
            
                    //nave jogador
                    DrawTexture(texNave, xNave, yNave, WHITE);
                    
                    //inimigos e disparos inimigos
                    for (x = 0; x < 5; x++){
                        if (inimigos[x].ativo == 1){
                            if (inimigos[x].tipo == 0){
                                DrawTexture(texInmg1, inimigos[x].xinmg, inimigos[x].yinmg, WHITE);
                            } else if (inimigos[x].tipo == 1){
                                DrawTexture(texInmg2, inimigos[x].xinmg, inimigos[x].yinmg, LIGHTGRAY);
                            } else if (inimigos[x].tipo == 2){
                                DrawTexture(texInmg3, inimigos[x].xinmg, inimigos[x].yinmg, GRAY);
                            } else if (inimigos[x].tipo == 3){
                                DrawTexture(texInmg4, inimigos[x].xinmg, inimigos[x].yinmg, GOLD);
                            }
                        }
                        for (y = 0; y < 10; y++){
                            if (inimigos[x].projeteis[y].ativo == 1){
                                DrawTexture(TexProjInmg, inimigos[x].projeteis[y].posxproj, inimigos[x].projeteis[y].posyproj, RED);
                                inimigos[x].projeteis[y].perigo = 1;
                            }
                        }
                    }
                    
                    //disparos do jogador
                    Rectangle recNave = {xNave, yNave, 20, 20};
                    for (x = 0; x < dificuldade; x++){
                        if (inimigos[x].ativo == 1){
                            
                            Rectangle recinmg = {inimigos[x].xinmg, inimigos[x].yinmg, 20, 20};
                            
                            if (CheckCollisionRecs(recNave, recinmg)){
                                
                                //COLISÃO ENTRE O PERSONAGEM PRINCIPAL E OS INIMIGOS
                                if (contcolisao == 0){
                                    PlaySound(danoSFX);
                                    vidas = vidas -1;
                                    contcolisao++;
                                } 
                                else if (contcolisao > 0 && contcolisao < 20){
                                    contcolisao++;
                                } 
                                else if (contcolisao == 20){
                                    contcolisao = 0;
                                }
                                
                                //SIMBORA
                            }
                        }
                        
                        for (y = 0; y < 10; y++){
                            if (inimigos[x].projeteis[y].ativo == 1){
                                Rectangle recproj = {inimigos[x].projeteis[y].posxproj, inimigos[x].projeteis[y].posyproj, 20,20};
                                if (CheckCollisionRecs(recNave, recproj)){
                                    if (inimigos[x].projeteis[y].perigo == 1){
                                        inimigos[x].projeteis[y].ativo = 0;
                                        inimigos[x].projeteis[y].posxproj = -20;
                                        inimigos[x].projeteis[y].posyproj =  -100;
                                        if (inimigos[x].tipo != 3){
                                            vidas = vidas - 1;
                                            score = score - 5;
                                            DrawTexture(texNave, xNave, yNave, RED);
                                        }
                                        else if (inimigos[x].tipo == 3){
                                            vidas = vidas - 2;
                                            score = score - 10;
                                            DrawTexture(texNave, xNave, yNave, RED);
                                        }
                                        PlaySound(danoSFX);
                                    }
                                    //COLISÃO ENTRE O PERSONAGEM PRINCIPAL E AS BALAS INIMIGAS
                                }
                            }
                        }
                    }
                    
                    //disparos jogador
                    for(i=0;i<disparosmax;i++){
                        if(disparos[i].ativo==true&&disparos[i].posy>0){ //disparos em movimento
                        
                            if(!pausado) disparos[i].posy -= disparos[i].speed;
                            
                            DrawTexture(texDisparo, disparos[i].posx, disparos[i].posy, GREEN);
                            
                            Rectangle hitbox = {disparos[i].posx+3, disparos[i].posy+2, 10, 12}; //hitbox jogador
                            
                            //checa colisões entre os disparos e os inimigos
                            for (x = 0; x < dificuldade; x++){
                                
                                Rectangle recinmg2 = {inimigos[x].xinmg, inimigos[x].yinmg+5, 33, 20};
                                if(CheckCollisionRecs(recinmg2, hitbox)){
                                    
                                    //COLISÃO ENTRE INIMIGOS E BALA DO PERSONAGEM PRINCIPAL
                                    DrawTexture(explos1, inimigos[x].xinmg+10, inimigos[x].yinmg+10, WHITE);
                                    DrawTexture(explos2, inimigos[x].xinmg+5, inimigos[x].yinmg+5, WHITE);
                                    PlaySound(killSFX);
                                    score = score + inimigos[x].pontuacao;
                                    
                                    disparos[i].ativo = false;
                                    inimigos[x].ativo = 0;
                                    
                                }
                            }
                        }
                        
                        if(disparos[i].ativo==true&&disparos[i].posy<=0){ //disparos finalizados
                            disparos[i].ativo = false;
                        }
                    }
                    // desenha 3 corações no canto inferior esquerdo da tela
                    
                    int heartSize = 20; // Tamanho do coração
                    int heartSpacing = 5; // Espaçamento entre corações
                    int heartPosY = GetScreenHeight() - heartSize - heartSpacing; // Coordenada Y dos corações
                    int heart1PosX = heartSpacing; // Coordenada X do primeiro coração
                    int heart2PosX = heartSpacing + heartSize + heartSpacing; // Coordenada X do segundo coração
                    int heart3PosX = heartSpacing + heartSize + heartSpacing + heartSize + heartSpacing; // Coordenada X do terceiro coração
                    //verifica a qtd de vida e desenha os coracoes na tela
                    if(vidas ==3){
                        DrawTexture(minhaTextura, heart1PosX, heartPosY, WHITE); // Desenha o primeiro coração
                        DrawTexture(minhaTextura, heart2PosX, heartPosY, WHITE); // Desenha o segundo coração
                        DrawTexture(minhaTextura, heart3PosX, heartPosY, WHITE); // Desenha o terceiro coração
                    }else if(vidas == 2){
                        // desenha 2 coraçoes
                        DrawTexture(minhaTextura, heart1PosX, heartPosY, WHITE); // Desenha o primeiro coração
                        DrawTexture(minhaTextura, heart2PosX, heartPosY, WHITE); // Desenha o segundo coraçãocoração
                    }else if(vidas == 1){
                        DrawTexture(minhaTextura, heart1PosX, heartPosY, WHITE); // Desenha o primeiro coração
                    }
                    
                    sprintf(score_c, "%d", score); //print do score
                    DrawText(score_c, larguraTela/2, alturaTela-25, 20, WHITE);
                    
                    break;
                
                case FIM:           
                    ClearBackground(BLACK);
                    DrawText("FIM DE JOGO", larguraTela/2 - 100, alturaTela/2, 30, GREEN);
                    DrawText("[Espaço] para voltar ao Menu", 5, alturaTela-25, 20, GREEN);
                    DrawText("Pontuação:", larguraTela/2 - 75, alturaTela/2 + 30, 20, GREEN);
                    DrawText(score_c, larguraTela/2 + 40, alturaTela/2 + 30, 20, GREEN);
                    break;

                default:
                    break;
            }
            
        EndDrawing();
    }
    //descarregamento de texturas
    UnloadTexture(fundoLOGO);
    UnloadTexture(texNave);
    UnloadTexture(texInmg1);
    UnloadTexture(texInmg2);
    UnloadTexture(texInmg3);
    UnloadTexture(texInmg4);
    UnloadTexture(texDisparo);
    UnloadTexture(TexProjInmg);
    UnloadTexture(minhaTextura);
    
    //descarregamento de sons
    UnloadSound(disparoSFX);
    UnloadSound(hitSFX);
    UnloadSound(deathSFX);
    UnloadSound(killSFX);
    UnloadMusicStream(temaMenu);
    UnloadMusicStream(tema1);
    UnloadMusicStream(tema2);
    
    CloseAudioDevice();

    CloseWindow();
   
    return 0;
}