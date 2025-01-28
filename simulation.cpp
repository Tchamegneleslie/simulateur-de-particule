
// ceci est la version finale du projet 

#include <iostream>
#include <fstream>
#include <vector>
#include <stdlib.h>
#include <ctime>
#include <unistd.h>


typedef struct  particules{
        int vx;
        int vy;
        unsigned int posx;
        unsigned int posy; // Coordonnee du centre
        unsigned int masse;
        int timelive;
}Particules;

void First_Init(std::vector<particules>& part, int nbrepart,const int WIDTH, const int HEIGHT){

  // std::vector<particules> cree un vecteur qui stocke un nombre de particule et dont la taille est modifiable

    srand(static_cast<unsigned int>(time(0)));
    int i = 0;
    for(i = 0; i < nbrepart; i++) {
        while(part[i].posx == 0 || part[i].posy == 0 || part[i].masse == 0 || part[i].timelive < 10 || part[i].posx > 10 || part[i].posy > 10 || part[i].masse > 10){
            part[i].vx = rand() %2;
            part[i].vy = rand() %2;
            part[i].posx = rand() %WIDTH;
            part[i].posy = rand() %HEIGHT;
            part[i].masse = rand() %3;
            part[i].timelive = rand() %40;
            if(part[i].vx == 0 && part[i].vy == 0){
                int s = rand() %1;
                if(s == 0)
                    part[i].vx = 1;
                else
                    part[i].vy = 1;   
            }

        }
    }
}


class Quadtree {
    private:
       
        int capacity;
        std::vector<std::pair<int, int>> points;
        bool divided;
        Quadtree *northwest;

    public:
        Quadtree( int capacity):capacity(capacity), divided(false), northwest(nullptr) {}

        bool insert(int px, int py) {

            if (points.size() < capacity) {
                points.emplace_back(px, py);
                return true;
            } else {
                
                return (northwest->insert(px, py));
            }
        } 

        void drawASCII(char** canvas) {
            // Marquer les points sur le canvas
            for (const auto& point : points) {
                canvas[point.second][point.first] = '*'; // Marquer le point
            }

            // Dessiner les sous-quadrants
            if (divided) {
                northwest->drawASCII(canvas);
                
            }
        }

        ~Quadtree() {
            delete northwest;
            
        }
};


void Draw_Interface(std::vector<particules> particule, const int WIDTH, const int HEIGHT, int nbrepart,char** canvas,Quadtree quadtree){
    int i = 0;

    int x;
    int y;
    
    // Insérer quelques points 
    for (i = 0; i < nbrepart; i++) {
        x = particule[i].posx;
        y = particule[i].posy;
        quadtree.insert(x, y);
    }
    for (i = 0; i < nbrepart; i++)
    {
        std::cout << "Particule " << i+1 << ":x= " << particule[i].posx << ",y= " << particule[i].posy << ",vx=" << particule[i].vx << ",vy=" << particule[i].vy <<",Livetime= "<< particule[i].timelive << " ,masse= " << particule[i].masse << std::endl;
    }
    

    // Dessiner le quadtree sur le canvas ASCII
  quadtree.drawASCII(canvas);
    

    // Afficher l'image ASCII
    for (int i = 0; i <= HEIGHT; ++i) {
        for (int j = 0; j <= WIDTH; ++j) {
            if(i == 0 || j == 0 || i == WIDTH || j == HEIGHT){
                canvas[i][j] = '#';
            }
            std::cout << canvas[i][j];
        }
        std::cout << std::endl;
    }
}

// fonction de collision sur les bords de la grille

void collision_border(std::vector<particules>& particule,int nbrepart, const int WIDTH, const int HEIGHT){

    int i;

    for(i = 0; i < nbrepart; i++) {
        if((particule[i].posx+1) > WIDTH-1 &&(particule[i].posx+1) > HEIGHT-1){
            particule[i].vx = (-particule[i].vx);
            particule[i].vy = (-particule[i].vy);
        }else if((particule[i].posx+1) > WIDTH-1)
            particule[i].vx = (-particule[i].vx);
        else if((particule[i].posy+1) > HEIGHT-1)
            particule[i].vy = (-particule[i].vy);
            
        if(particule[i].posx-1 < 1  && particule[i].posy-1 < 1){
            particule[i].vx = (-particule[i].vx);
            particule[i].vy = (-particule[i].vy);
        }else if(particule[i].posx-1 == 0){
            if(particule[i].vx < 0 ||  particule[i].vy < 0)
                particule[i].vx = (-particule[i].vx);
        }else if(particule[i].posy-1 == 0){
            if(particule[i].vx < 0 ||  particule[i].vy < 0)
                particule[i].vy = (-particule[i].vy);
        }
    }
}

// fonction de collision entre les particules

void collision_of_particule(std::vector<particules>& particule,int& nbrepart){
    
    int i;
    int j;
    int k = 0;

    for(i = 0; i < nbrepart; i++) {
        for(j = 1; j < nbrepart; j++){
            if(j == i)
                j++;
            if(particule[i].posx == particule[j].posx && particule[i].posy == particule[j].posy){
                if(particule[j].masse > particule[i].masse ){
                    particule.erase(particule.begin() + i);
                nbrepart -= 1;
                }else{
                    particule.erase(particule.begin() + j);
                    nbrepart -= 1;
                }
                if (particule[j].masse == particule[i].masse) {
                    particule[j].posx -= particule[j].vx;
                    particule[i].posx += particule[i].vx;
                }
                    
            }
                
        }
    }
}

void Deplacement(std::vector<particules>& particule,int& nbrepart, const int WIDTH, const int HEIGHT, Quadtree quadtree,char** canvas){
    int i;
    int j = 0;
    int k = 0;
    int l = 0;
    int s = 40;

    while(s != 0){
    
        collision_border(particule,nbrepart,WIDTH,HEIGHT);
        collision_of_particule(particule,nbrepart);

        sleep(1);
        system("CLS");
        for (i = 0; i <= HEIGHT; ++i) {
            canvas[i] = new char[WIDTH];
            std::fill(canvas[i], canvas[i] + WIDTH, ' '); // Remplir avec des espaces
        }    
        Draw_Interface(particule, WIDTH, HEIGHT, nbrepart,canvas,quadtree);

        for(j = 0; j < nbrepart; j++){
            particule[j].posx += particule[j].vx; 
            particule[j].posy += particule[j].vy;
            particule[j].timelive -= 1;
            if(particule[j].timelive == 0){
                particule.erase(particule.begin() + j);
                nbrepart -= 1;
            }
        }
        s--;
        std::cout << s;
       
    }
    
}

int main(){
    srand(static_cast<unsigned int>(time(0)));

    const int WIDTH = 20; // Largeur de l'image ASCII
    const int HEIGHT = 20; // Hauteur de l'image ASCII

    int i = 0;
    int j = 0;

    int t = 0;

    int nbrepart = 9 ;
    
    std::vector<particules> particule(nbrepart);
    for(const auto& particules : particule){
        particule.push_back({1,1,0,0,0,0}); 
    }

    // Créer un quadtree avec une zone carrée 
    Quadtree quadtree( nbrepart); 
    char** canvas = new char*[HEIGHT];
    
    for (int i = 0; i <= HEIGHT; ++i) {
        canvas[i] = new char[WIDTH];
        std::fill(canvas[i], canvas[i] + WIDTH, ' '); // Remplir avec des espaces
    }

    First_Init(particule, nbrepart, WIDTH, HEIGHT);


    Draw_Interface(particule, WIDTH, HEIGHT, nbrepart,canvas,quadtree);

    Deplacement(particule,nbrepart,WIDTH,HEIGHT,quadtree,canvas);

    // Libérer la mémoire
    for (int i = 0; i < HEIGHT; ++i) {
        delete[] canvas[i];
    }
    delete[] canvas;
    quadtree.~Quadtree();

    return 0;
    
}
