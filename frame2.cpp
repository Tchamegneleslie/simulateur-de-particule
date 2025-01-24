#include <iostream>
#include <fstream>
#include <vector>
#include <random>
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

void First_Init(std::vector<particules>& part, int nbrepart){
    srand(static_cast<unsigned int>(time(0)));
    int i = 0;
    for(i = 0; i < nbrepart; i++) {
        while(part[i].posx == 0 || part[i].posy == 0 || part[i].masse == 0 || part[i].timelive < 5 || part[i].posx > 10 || part[i].posy > 10 || part[i].masse > 10){
            part[i].vx = 1;
            part[i].vy = 1;
            part[i].posx = rand() %7;
            part[i].posy = rand() %7;
            part[i].masse = rand() %3;
            part[i].timelive = 20;

        }
    }
}

class Rectangle {
public:
    int x, y, width, height;

    Rectangle(int x, int y, int width, int height)
        : x(x), y(y), width(width), height(height) {}

    bool contains(int px, int py) {
        return (px >= x && px < x + width && py >= y && py < y + height);
    }
};

class Quadtree {
    private:
        Rectangle boundary;
        int capacity;
        std::vector<std::pair<int, int>> points;
        bool divided;
        Quadtree *northwest, *northeast, *southwest, *southeast;

    public:
        Quadtree(Rectangle boundary, int capacity): boundary(boundary), capacity(capacity), divided(false), northwest(nullptr), northeast(nullptr), southwest(nullptr), southeast(nullptr) {}

        void subdivide() {
            int x = boundary.x;
            int y = boundary.y;
            int w = boundary.width / 2;
            int h = boundary.height / 2;

            northwest = new Quadtree(Rectangle(x, y, w, h), capacity);
            northeast = new Quadtree(Rectangle(x + w, y, w, h), capacity);
            southwest = new Quadtree(Rectangle(x, y + h, w, h), capacity);
            southeast = new Quadtree(Rectangle(x + w, y + h, w, h), capacity);

            divided = true;
        }

        bool insert(int px, int py) {
            // if (!boundary.contains(px, py)) {
            //     return false;
            // }

            if (points.size() < capacity) {
                points.emplace_back(px, py);
                return true;
            } else {
                if (!divided) {
                    subdivide();
                }
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
                northeast->drawASCII(canvas);
                southwest->drawASCII(canvas);
                southeast->drawASCII(canvas);
            }
        }

        ~Quadtree() {
            delete northwest;
            delete northeast;
            delete southwest;
            delete southeast;
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
        std::cout << "Particule " << i+1 << ": x= " << particule[i].posx << ",y= " << particule[i].posy << ",vx= " << particule[i].vx << ",vy= " << particule[i].vy <<",Livetime= "<< particule[i].timelive << std::endl;
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

void Deplacement(std::vector<particules>& particule,int nbrepart, const int WIDTH, const int HEIGHT, Quadtree quadtree,char** canvas){
    int i;
    int j = 0;
    int k = 0;
    int s = 0;

    for(i = 0; i < nbrepart; i++){
        if(particule[i].timelive > particule[k].timelive)
            k = i;
        s = particule[k].timelive;
    }

    while(s != 0){
    
        for(i = 0; i < nbrepart; i++) {
            if((particule[i].posx+1) == WIDTH || (particule[i].posx+1) > WIDTH)
                particule[i].vx = (-particule[i].vx);
            if((particule[i].posy+1) == HEIGHT || (particule[i].posy+1) > HEIGHT)
                particule[i].vy = (-particule[i].vy);
            if((particule[i].posx-1 == 0 || particule[i].posx-1 < 0) && particule[i].vx < 0)
                particule[i].vx = (-particule[i].vx);
            if((particule[i].posy-1 == 0 || particule[i].posy-1 < 0) && particule[i].vx < 0)
                particule[i].vy = (-particule[i].vy);
        }

        sleep(1);
        system("CLS");
        for (int i = 0; i <= HEIGHT; ++i) {
            canvas[i] = new char[WIDTH];
            std::fill(canvas[i], canvas[i] + WIDTH, ' '); // Remplir avec des espaces
        }

        for(j = 0; j < nbrepart; j++){
            particule[j].posx += particule[j].vx; 
            particule[j].posy += particule[j].vy;
            particule[j].timelive -=1;
            if(particule[j].timelive == 0)
                particule.erase(particule.begin() + j);
        }
    
        Draw_Interface(particule, WIDTH, HEIGHT, nbrepart,canvas,quadtree);
        
        s--;
    }
    
}

int main(){
    srand(static_cast<unsigned int>(time(0)));

    const int WIDTH = 15; // Largeur de l'image ASCII
    const int HEIGHT = 15; // Hauteur de l'image ASCII

    int i = 0;
    int j = 0;

    int t = 0;

    int nbrepart = 5 ;
    
    std::vector<particules> particule(nbrepart);
    for(const auto& particules : particule){
        particule.push_back({1,1,0,0,0,0}); 
    }

    // Créer un quadtree avec une zone carrée 
    Quadtree quadtree(Rectangle(0, 0, WIDTH, HEIGHT), nbrepart); 
    char** canvas = new char*[HEIGHT];
    
    for (int i = 0; i <= HEIGHT; ++i) {
        canvas[i] = new char[WIDTH];
        std::fill(canvas[i], canvas[i] + WIDTH, ' '); // Remplir avec des espaces
    }

    First_Init(particule, nbrepart);

    system("CLS");

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