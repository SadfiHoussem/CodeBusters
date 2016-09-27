#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <limits>

using namespace std;

/**
 * Send your busters out into the fog to trap ghosts and bring them home!
 **/
 
 class Ghost {
 public:
    int id;
    int x;
    int y;
    int value;
    int state;
    
    Ghost(int _id, int _x, int _y, int _state, int _value) {
        id = _id;
        x = _x;
        y = _y;
        value = _value;
        state = _state;
    }
    
    float distance (int x1, int y1) {
        float diffY = y - y1;
        float diffX = x - x1;
        return sqrt((diffY * diffY) + (diffX * diffX));
    }
    
 };
 
 class Buster {
 public:
    int id;
    int x;
    int y;
    int state;
    int value;
    int idAction; // Targeted Ghost
    bool busy;
    int stun;
    int isSafe; // 0 : Safe otherwise number of busters near me
    
    Buster(int _id, int _x, int _y, int _state, int _value) {
        id = _id;
        x = _x;
        y = _y;
        state = _state;
        value = _value;
        idAction = -1;
        busy = false;
        stun = 20;
    }
    
    float distance (int x1, int y1) {
        float diffY = y - y1;
        float diffX = x - x1;
        return sqrt((diffY * diffY) + (diffX * diffX));
    }
    
    bool amIHome(int x, int y) {
        int aux = (int) distance(x,y);
        return aux <= 1580;
    }
    
    bool catchHim(Ghost g) {
        int aux = (int) distance(g.x,g.y);
        return (aux <= 1760)&&(aux>=900);
    }
    
    void isMyGhostAlive(vector<Ghost> ghosts) {
        if (idAction!=-1) {
            int i = idAction;
            auto it = find_if(ghosts.begin(), ghosts.end(), [&i](const Ghost& ghost) {return ghost.id == i;});
            if (it == ghosts.end())
            {
                idAction=-1;
            }
        }
    }
    
    bool isDangerous() {
        if (stun == 20) {
            return true;
        }
        return false;
    }
    
    int nearbyEnemyLocal(vector<Buster> enemyBustersBrut) {
        int nbrEnemy = 0;
        for (int b=0; b<enemyBustersBrut.size();b++) {
            if ((enemyBustersBrut.at(b).state!=2)&&(enemyBustersBrut.at(b).isDangerous())&&(enemyBustersBrut.at(b).distance(x,y)<=2200)) {
                nbrEnemy += 1;
            }
        }
        return nbrEnemy;
    }
    
    int closeEnemyLocal(vector<Buster> enemyBustersBrut) {
        int nbrEnemy = 0;
        int idRes = -1;
        for (int b=0; b<enemyBustersBrut.size();b++) {
            if ((enemyBustersBrut.at(b).isDangerous())&&(enemyBustersBrut.at(b).distance(x,y)<=1760)) {
                nbrEnemy += 1;
                idRes = enemyBustersBrut.at(b).id;
            }
        }
        if (nbrEnemy == 1) {
            return idRes;
        } else {
            return -1;
        }
    }
    
 };
 
 class Spot {
 public:
    int x;
    int y;
    int lightness;
    int idBuster;
    
    Spot(int _x,int _y) {
        x = _x;
        y = _y;
        lightness = 1; // 1 is Dark | 10 is light
        idBuster = -1;
    }
 };
 
 class Solver {
 public:
    int bustersPerPlayer;
    int ghostCount;
    int myTeamId;
    int xBase;
    int yBase;
    int maxGhostWeight;
    int ghostEaten;
    int nbrTurn;
    

    vector<Buster> myBusters;
    vector<Buster> enemyBusters;
    vector<Ghost> ghosts;
    vector<Ghost> ghostsBrut; // Raw data
    vector<Buster> enemyBustersBrut; // Raw data
    vector<Spot> spots;
    
    Solver() {
        Spot p1(4500,1500);
        spots.push_back(p1);
        Spot p5(8500,1000);
        spots.push_back(p5);
        Spot p7(12500,1000);
        spots.push_back(p7);
        Spot p8(14500,1000);
        spots.push_back(p8);
        Spot p2(2000,3500);
        spots.push_back(p2);
        Spot p10(6000,3500);
        spots.push_back(p10);
        Spot p12(9000,3500);
        spots.push_back(p12);
        Spot p13(12000,3500);
        spots.push_back(p13);
        Spot p29(15000,3500);
        spots.push_back(p29);
        Spot p21(2000,5000);
        spots.push_back(p21);
        Spot p23(6000,5000);
        spots.push_back(p23);
        Spot p25(9000,5000);
        spots.push_back(p25);
        Spot p26(12000,5000);
        spots.push_back(p26);
        Spot p28(15000,5000);
        spots.push_back(p28);
        Spot p3(1200,7500);
        spots.push_back(p3);
        Spot p16(6000,7000);
        spots.push_back(p16);
        Spot p18(10000,7000);
        spots.push_back(p18);
        Spot p20(14000,7500);
        spots.push_back(p20);
        
        maxGhostWeight = std::numeric_limits<float>::max();
        ghostEaten = 0;
        nbrTurn = 0;
        
    }
    
    Ghost findGhostById(int id) {
        auto it = find_if(ghosts.begin(), ghosts.end(), [&id](const Ghost& ghost) {return ghost.id == id;});
        Ghost g(-1, -1, -1, -1, -1);
        if (it != ghosts.end())
        {
            auto index = std::distance(ghosts.begin(), it);
            g.id = id;
            g.x = ghosts.at(index).x;
            g.y = ghosts.at(index).y;
            g.value = ghosts.at(index).value;
        }
        return g;
    }
    
    void addGhost(Ghost g) {
        int i = g.id;
        auto it = find_if(ghosts.begin(), ghosts.end(), [&i](const Ghost& ghost) {return ghost.id == i;});
        if (it != ghosts.end())
        {
            auto index = std::distance(ghosts.begin(), it);
            ghosts.at(index).x = g.x;
            ghosts.at(index).y = g.y;
            ghosts.at(index).value = g.value;
            ghosts.at(index).state = g.state;
        } else {
            ghosts.push_back(g);
        }
    }
    
    void addMyBuster(Buster b) {
        int i = b.id;
        auto it = find_if(myBusters.begin(), myBusters.end(), [&i](const Buster& buster) {return buster.id == i;});
        
        if (it != myBusters.end())
        {
            auto index = std::distance(myBusters.begin(), it);
            myBusters.at(index).x = b.x;
            myBusters.at(index).y = b.y;
            myBusters.at(index).state = b.state;
            myBusters.at(index).value = b.value;
        } else {
            myBusters.push_back(b);
        }
        
    }
    
    void addEnemyBuster(Buster b) {
        int i = b.id;
        auto it = find_if(enemyBusters.begin(), enemyBusters.end(), [&i](const Buster& buster) {return buster.id == i;});
        
        if (it != enemyBusters.end())
        {
            auto index = std::distance(enemyBusters.begin(), it);
            enemyBusters.at(index).x = b.x;
            enemyBusters.at(index).y = b.y;
            enemyBusters.at(index).state = b.state;
            enemyBusters.at(index).value = b.value;
        } else {
            enemyBusters.push_back(b);
        }
        
    }
    
    void killGhost(int i) {
        auto it = find_if(ghosts.begin(), ghosts.end(), [&i](const Ghost& ghost) {return ghost.id == i;});
        
        if (it != ghosts.end())
        {
            auto index = std::distance(ghosts.begin(), it);
            ghosts.erase(ghosts.begin() + index);
        }
    }
    
    void manageStamina(int idGhost) {
        
        for (int g=0;g<ghosts.size();g++) {
            if (ghosts.at(g).id==idGhost) {
                ghosts.at(g).state -= 1;
                break;
            }
        }
    }
    
    int nbrBusterDrainGhost(int idGhost) { // My Busters Not all Busters
        int nbr = 0;
        for (int i=0; i<myBusters.size(); i++) {
            if ((myBusters.at(i).state==3)&&(myBusters.at(i).value==idGhost)) {
                nbr = nbr +1;
            }
        }
        return nbr;
    }
    
    string chooseGhost(Buster* b) {
        float weight = std::numeric_limits<float>::max();
        int xR=-1;
        int yR=-1;
        int idR=-1;
        for (auto i = ghosts.begin() ; i != ghosts.end() ; i++) {
            float distFromBusterToGhost = i->distance(b->x,b->y);
            if (distFromBusterToGhost-800>=900) { // The buster advance 800 units 
                int nbrBDGhost = nbrBusterDrainGhost(i->id);
                float distFromBaseToGhost = i->distance(xBase,yBase);
                float aux;
                
                if ((i->state == 0)&&(nbrBDGhost == 0)) {
                    aux = distFromBusterToGhost + (800 * 1) + distFromBaseToGhost; 
                }
                else if ((i->state - (distFromBusterToGhost/800*nbrBDGhost))==0) {
                    aux = std::numeric_limits<float>::max();
                }
                else {
                    aux = distFromBusterToGhost + (800 * (i->state - (distFromBusterToGhost/800*nbrBDGhost))) + distFromBaseToGhost; 
                }
                
                if ((i->state<=maxGhostWeight)&&(aux<weight)) {
                    weight = aux;
                    xR = i->x;
                    yR = i->y;
                    idR = i->id;
                }
            }
        }
        if (idR!=-1) {
            b->idAction=idR;
            return "MOVE "+ std::to_string(xR) + " " + std::to_string(yR);
        }
        return "-1";
        
    }
    
    void updateLightness() {
        for (int s=0; s<spots.size(); s++) {
            if (spots.at(s).lightness > 1) {
                spots.at(s).lightness --;
            }
            if (spots.at(s).lightness>=250) {
                spots.at(s).idBuster = -1;
            }
            for (int b=0; b<myBusters.size(); b++) {
                if (myBusters.at(b).distance(spots.at(s).x,spots.at(s).y)<=350) {
                    spots.at(s).lightness = 350;
                    break;
                }
            }
        }
    }
    
    void updateCapturedGhosts() {
        for(int i=0; i<enemyBusters.size(); i++) {
            if (enemyBusters.at(i).state == 1) { // Carrying a ghost
                killGhost(enemyBusters.at(i).value);
            }
        }
        for(int i=0; i<myBusters.size(); i++) {
            if (myBusters.at(i).state == 1) { // Carrying a ghost
                killGhost(myBusters.at(i).value);
            }
        }
    }
    
    bool percentExplored(int percent) {
        int averageLightness = 0;
        for (int i=0; i<spots.size(); i++) {
            averageLightness += spots.at(i).lightness;
        }
        averageLightness = averageLightness / spots.size() *100 / 350;
        
        if (averageLightness>=percent) {
            return true;
        } else {
            return false;
        }
    }
    
    void updateGhostWeight(int maxStamina) {
        if ((ghostEaten>=(ghostCount/bustersPerPlayer))||((nbrTurn*2)>=(450/bustersPerPlayer))||(percentExplored(70))) {
            maxGhostWeight = 40;
        }
        else if (percentExplored(bustersPerPlayer*7)||((nbrTurn*2)>=(120/bustersPerPlayer))) {
            maxGhostWeight = 15;
            
        }
        else {
            maxGhostWeight = maxStamina;
            
        }
    }
    
    Spot busterHaveSpot(int idBuster) {
        Spot s(-1,-1);
        for (int i=0;i<spots.size();i++) {
            if (spots.at(i).idBuster == idBuster) {
                return spots.at(i);
            }
        }
        return s;
    }
    
    
    string findGhosts(Buster* b) {
        Spot s = busterHaveSpot(b->id);
        if ((s.x == -1)&&(s.y == -1)) {
            int minWeight = std::numeric_limits<int>::max();;
            int index=-1;
            for (int i=0; i<spots.size();i++) {
                if (spots.at(i).idBuster == -1) {
                    int d = b->distance(spots.at(i).x,spots.at(i).y)+100;
                    int aux = (d*spots.at(i).lightness);
                    if ((d>=2100)&&(minWeight>aux)) {
                        index = i;
                        minWeight = aux;
                    }
                }
            }
            spots.at(index).idBuster = b->id;
            return "MOVE " + std::to_string(spots.at(index).x) + " " + std::to_string(spots.at(index).y);
        } else {
            return "MOVE " + std::to_string(s.x) + " " + std::to_string(s.y);
        }
    }
    
    bool targetExist(int idGhost) {
        auto it = find_if(ghostsBrut.begin(), ghostsBrut.end(), [&idGhost](const Ghost& ghost) {return ghost.id == idGhost;});
        
        if (it != ghostsBrut.end())
        {
            return true;
        } else {
            return false;
        }
    }
    
    string behaviorTreeNormalState(Buster* b) {
        if (b->state == 1) { // Buster carrying a ghost
            if ((b->amIHome(xBase, yBase))) { // If buster is home release the ghost
                b->busy = false;
                b->idAction = -1;
                ghostEaten += 1;
                return "RELEASE";
            }
            return "MOVE " + std::to_string(xBase) + " " + std::to_string(yBase);
            
        } else if ((b->state == 0)||(b->state == 3)) { //  Buster is moving or trapping a ghost
            b->isMyGhostAlive(ghosts);
            if ((ghosts.size()!=0)&&(b->idAction!=-1)&&(b->catchHim(findGhostById(b->idAction)))) { // The target is in range
                if (targetExist(b->idAction)) { // The target still exist
                    b->busy = true;
                    manageStamina(b->idAction);
                    return "BUST "+ std::to_string(b->idAction);       
                } else { // Search for another ghost
                    killGhost(b->idAction);
                    string aux = chooseGhost(b);
                    if (aux=="-1") {
                        return findGhosts(b);
                    }
                    return aux;
                }
                
            } else {
                if (ghosts.size()==0) { // Search for ghosts
                    return findGhosts(b);
                }
                string aux = chooseGhost(b);
                if (aux=="-1") {
                    return findGhosts(b);
                }
                return aux;
            }
        }
        return "MOVE " + std::to_string(b->x) + " " + std::to_string(b->y);
    }
    
    string behaviorTreeSelfDefenseState(Buster* b,Buster e) {
        if (b->state!=1) {
            b->stun = 0;
            return "STUN "+std::to_string(e.id);
        } else {
            b->busy = false;
            addLostGhost(b);
            b->idAction = -1;
            b->stun = 0;
            return "STUN "+std::to_string(e.id);    
        }
    }
    
    void addLostGhost(Buster* b) {
        if ((b->idAction!=-1)&&(!(b->amIHome(xBase, yBase)))) {
            Ghost g(b->idAction,b->x,b->y,1,0);
            addGhost(g);
        }
    }
    
    Buster nearbyEnemy(Buster myB) { // Priority for the one carrying a ghost
        Buster e(-1,-1,-1,-1,-1);
        for (int b=0; b<enemyBustersBrut.size();b++) {
            if ((enemyBustersBrut.at(b).state!=2)&&(enemyBustersBrut.at(b).distance(myB.x,myB.y)<=1760)) {
                if (enemyBustersBrut.at(b).state == 1) {
                    e.id = enemyBustersBrut.at(b).id;
                    e.x = enemyBustersBrut.at(b).x;
                    e.y = enemyBustersBrut.at(b).y;
                    e.state = enemyBustersBrut.at(b).state;
                    e.value = enemyBustersBrut.at(b).value;
                    return e;
                }
                e.id = enemyBustersBrut.at(b).id;
                e.x = enemyBustersBrut.at(b).x;
                e.y = enemyBustersBrut.at(b).y;
                e.state = enemyBustersBrut.at(b).state;
                e.value = enemyBustersBrut.at(b).value;
                return e;
            }
        }
        return e;
    }
    
    Buster nearbyEnemyCarryGhost(Buster myB) {
        Buster e(-1,-1,-1,-1,-1);
        for (int b=0; b<enemyBustersBrut.size();b++) {
            if ((enemyBustersBrut.at(b).state==1)&&(enemyBustersBrut.at(b).distance(myB.x,myB.y)<=4000)) {
                e.id = enemyBustersBrut.at(b).id;
                e.x = enemyBustersBrut.at(b).x;
                e.y = enemyBustersBrut.at(b).y;
                e.state = enemyBustersBrut.at(b).state;
                e.value = enemyBustersBrut.at(b).value;
                return e;
            }
        }
        return e;
    }
    
    void promoteSpots() {
        int aux;
        if ((xBase == 0)&&(yBase == 0)) {
            aux = 5000;
        } else {
            aux = 3000;
        }
        for (int i=0; i<spots.size(); i++) {
            if ((spots.at(i).x<=5500)||(spots.at(i).x>=10500)) {
                spots.at(i).lightness = 20;
            }
        }
    }
    
    void reloadGun(Buster* b) {
        if (b->stun !=20) {
            b->stun += 1;
        }
    }
    
    void reloadEnemyGun() {
        for (int b=0; b<enemyBusters.size();b++) {
            if (enemyBusters.at(b).stun !=20) {
                enemyBusters.at(b).stun += 1;
            }
        }    
    }
    
    void complain(int idCloseEnemy) {
        for (int b=0; b<enemyBusters.size();b++) {
            if (enemyBusters.at(b).id == idCloseEnemy) {
                enemyBusters.at(b).stun = 0;
            }
        }
    }
    
    bool targetAttacked(vector<int> smallMemory,int idEnemy) {
        for (int i=0;i<smallMemory.size();i++) {
            if (smallMemory.at(i)==idEnemy) {
                return true;
            }
        }
        return false;
    }
    
    void solve() {
        nbrTurn += 1;
        vector<int> smallMemory; // Prevent from attack same target twice
        for (auto i = myBusters.begin() ; i != myBusters.end() ; i++) {
            updateLightness();
            updateGhostWeight(3);
            updateCapturedGhosts();
            reloadGun(&(*i));
            percentExplored(60);
            int idCloseEnemy = i->closeEnemyLocal(enemyBustersBrut);
    
            if ((i->state == 2)&&(idCloseEnemy!=-1)) {
                complain(idCloseEnemy);
                cout << "MOVE " + std::to_string(i->x) + " " + std::to_string(i->y) << endl;
            } else {
                Buster e = nearbyEnemy(*i);
                Buster eg = nearbyEnemyCarryGhost(*i);
                
                if ((i->state!=2)&&(i->stun == 20)&&((e.x!=-1)&&(e.y!=-1))&&(!targetAttacked(smallMemory,e.id))) { // Nearby Enemy (And/Or Helping friend NOT YET)
                    smallMemory.push_back(e.id);
                    cout << behaviorTreeSelfDefenseState(&(*i),e) << endl;
                } else if (((i->state == 0)||(i->state == 3))&&(i->stun == 20)&&(eg.x!=-1)&&(eg.y!=-1)) {
                    cout << "MOVE " + std::to_string(eg.x) + " " + std::to_string(eg.y) << endl;
                } else {
                    cout << behaviorTreeNormalState(&(*i)) << endl;
                }
            }
        }
        reloadEnemyGun();
    }
 };
 
int main()
{
    int bustersPerPlayer; // the amount of busters you control
    cin >> bustersPerPlayer; cin.ignore();
    int ghostCount; // the amount of ghosts on the map
    cin >> ghostCount; cin.ignore();
    int myTeamId; // if this is 0, your base is on the top left of the map, if it is one, on the bottom right
    cin >> myTeamId; cin.ignore();
    
    Solver s;
    s.bustersPerPlayer = bustersPerPlayer;
    s.ghostCount = ghostCount;
    s.myTeamId = myTeamId;
    
    if (myTeamId) {
        s.xBase = 16000;
        s.yBase = 9000;
    } else {
        s.xBase = 0;
        s.yBase = 0;
    }
    
    s.promoteSpots();

    // game loop
    while (1) {
        vector<Ghost> ghostsBrut;
        vector<Buster> enemyBustersBrut;
        int entities; // the number of busters and ghosts visible to you
        cin >> entities; cin.ignore();
        for (int i = 0; i < entities; i++) {
            int entityId; // buster id or ghost id
            int x;
            int y; // position of this buster / ghost
            int entityType; // the team id if it is a buster, -1 if it is a ghost.
            int state; // For busters: 0=idle, 1=carrying a ghost.
            int value; // For busters: Ghost id being carried. For ghosts: number of busters attempting to trap this ghost.
            cin >> entityId >> x >> y >> entityType >> state >> value; cin.ignore();
            
            if (entityType==-1) {
                Ghost g(entityId, x, y, state, value);
                ghostsBrut.push_back(g);
                s.addGhost(g);
            }
            else if (entityType==myTeamId) {
                Buster b(entityId, x, y, state, value);
                s.addMyBuster(b);
            }
            else {
                Buster b(entityId, x, y, state, value);
                enemyBustersBrut.push_back(b);
                s.addEnemyBuster(b);
            }
        }
        s.enemyBustersBrut = enemyBustersBrut;
        s.ghostsBrut = ghostsBrut;
        
        s.solve();
        
    }
}
