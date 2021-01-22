#include <iostream>
using namespace std;

template<typename K, typename V>
class Two4Tree {

    private:

        //structure of a 2-4 node
        struct Two4Node {
            Two4Node *child[4]; //the children of the node
            int numkeys;//number of keys currently filled in node
            K keys[3];//array of keys in node
            V values[3];// values attached to keys in node
            bool leaf;//whether it is a leaf or not
            int sub;
        };//end of Two4Node

        Two4Node* root;
        int sz;

        Two4Node* newNode(){
            Two4Node *newnode = new Two4Node;
            //no children
            newnode -> sub = 0;
            newnode -> numkeys = 0; //always start with one key
            newnode -> leaf = true;//always start as leaf
            return newnode;
        }//end of newNode

        //makes a new 2-4 node using key and val
        Two4Node* newNode(K key, V val){
            Two4Node *newnode = new Two4Node;
            //no children
            newnode -> sub = 1;
            newnode -> numkeys = 1; //always start with one key
            //insert key and val
            newnode -> keys[0] = key;
            newnode -> values[0] = val;
            newnode -> leaf = true;//always start as leaf
            return newnode;
        }//end of newNode

        Two4Node* copy(Two4Node* curr){
            Two4Node* n = new Two4Node();
            
            for (int i = 0; i < curr -> numkeys; i++){n -> keys[i] = curr -> keys[i]; n -> values[i] = curr -> values[i];}
            for (int i = 0; i <= curr -> numkeys; i++){if(!(curr -> leaf)) n -> child[i] = copy(curr -> child[i]);}
            
            n -> leaf = curr -> leaf;
            n -> sub = curr -> sub;
            n -> numkeys = curr -> numkeys;
            return n;
        }
        Two4Node& operator= (const Two4Node& right){
            Two4Node* ret = copy(right);
            return ret;
        }

        void clearMem(Two4Node* curr){
            if (curr -> leaf) return;//tree is empty
            for (int i = 0; i <= curr -> numkeys; i++) if (curr -> child[i] != NULL){ clearMem(curr -> child[i]); free(curr -> child[i]);}
            curr -> leaf = true; 
            return;
        }//end of clearMem

        
        void splitChild(Two4Node* curr, int chind){
            Two4Node* z = new Two4Node;
            Two4Node* chi = curr -> child[chind];
            z -> leaf = chi -> leaf;
            z -> numkeys = 1;
            z -> keys[0] = chi -> keys[2];
            z -> values[0] = chi -> values[2];
            if (!(chi -> leaf)){
                for (int i = 0; i < chi -> numkeys + 1; i++) if (chi -> child[i + 2] != NULL) z -> child[i] = chi -> child[i + 2]; 
            }
            chi -> numkeys = 1;
            for (int j = curr -> numkeys; j >= chind + 1; j--) curr -> child[j + 1] = curr -> child[j];
            curr -> child[chind + 1] = z;
            for (int j = curr -> numkeys - 1; j >= chind; j--){curr -> keys[j + 1] = curr -> keys[j]; curr -> values[j + 1] = curr -> values[j];}
            curr -> keys[chind] = chi -> keys[1];
            curr -> values[chind] = chi -> values[1];
            sz++;
            curr ->numkeys++;
            if (z -> leaf) z -> sub = z -> numkeys;
            else {
                z -> sub = 0;
                for(int i = 0; i < z -> numkeys + 1; i++) z -> sub += z -> child[i] -> sub;
                z -> sub += z -> numkeys;
            }
            if (chi -> leaf) chi -> sub = chi -> numkeys;
            else {
                chi -> sub = 0;
                for(int i = 0; i < chi -> numkeys + 1; i++) chi -> sub += chi -> child[i] -> sub;
                chi -> sub += chi -> numkeys;
            }
            //curr -> sub = z -> sub + chi -> sub + curr -> numkeys;
        }//end of splitChild

        void insertNonfull(Two4Node* curr, K key, V val){
            
            int i = curr -> numkeys - 1;
            if (curr -> leaf){
                
                while (i >= 0 && key < curr -> keys[i]) {curr -> keys[i + 1] = curr -> keys[i]; curr -> values[i + 1] = curr ->values[i]; i--;}
                curr -> keys[i + 1] = key;
                curr -> values[i + 1] = val;
                
                curr -> numkeys++;
                curr -> sub++;
            }//end of if
            else{
                while (i >= 0 && key < curr -> keys[i]) i--;
                i++;
                if (curr -> child[i] -> numkeys == 3){
                    splitChild(curr, i);
                    if (key > curr -> keys[i]) i++;
                }//end of if
                curr -> sub++;
                insertNonfull(curr -> child[i], key, val);
            }//end of else
            
        }//end of insertNonfull

        Two4Node* insert(Two4Node* curr, K key, V val){//returns pointer to root
            if (curr == NULL) {sz = 1; return newNode(key, val);}

            if (curr -> numkeys == 3){
                sz++;
                Two4Node* s = newNode();
                s -> leaf = false;
                s -> child[0] = curr;
                splitChild(s, 0);
                insertNonfull(s, key, val);
                s -> sub = s -> numkeys;
                for (int i = 0; i < s -> numkeys + 1; i++)s -> sub += s ->child[i] -> sub;
                return s;
            }//end of insertNode
            else{insertNonfull(curr, key, val); return curr;}
        }//end of insertNode
        
        Two4Node* nodeDelete(Two4Node* curr, K key){
            curr -> sub--;
            if(curr -> leaf){
                int tr = 0;
                for(tr; tr < curr -> numkeys; tr++)if (curr -> keys[tr] == key) break;
                for(tr; tr < curr -> numkeys - 1; tr++){curr ->keys[tr] = curr -> keys[tr + 1]; curr ->values[tr] = curr -> values[tr + 1];}
                curr -> numkeys--;
                return this -> root;
            }
            
            int j = 0;
            while (j < curr -> numkeys && curr -> keys[j] < key) j++;//find the correct child to look under
            bool last=true, first=true;//to figure out where child is
            //set first and last
            if (j != curr -> numkeys) last = false;
            if (j != 0) first = false;

            int t;
            if (!last) t = j + 1;//default t to be after j
            else t = j - 1;//unless j is last
           
            
            if (j != curr -> numkeys && curr -> keys[j] == key){
                int pnum, snum;
                if (t == j + 1){
                    pnum = j;
                    snum = t;
                }
                else{pnum = t; snum = j;}
                
                if (curr -> child[j] -> numkeys >= 2){
                    K temp = predDel(key, curr, curr -> child[pnum]);
                    curr -> keys[pnum] = temp;
                    curr -> values[pnum] = *(nodeSearch(temp, curr -> child[pnum]));
                    return nodeDelete(curr -> child[pnum], temp);
                }
                else if ((!last && curr -> child[t] -> numkeys >=2) || (!first && curr -> child[t]  -> numkeys >=2)){
                    K temp = succDel(key, curr, curr -> child[snum]);
                    curr -> keys[pnum] = temp;
                    curr -> values[pnum] = *(nodeSearch(temp, curr -> child[snum]));
                    return nodeDelete(curr -> child[snum], temp);
                }
                else{
                    if (t == j + 1){//merge left
                        Two4Node* y = curr -> child[j];
                        Two4Node* z = curr -> child[t];
                        return merge(y, curr, z, j, key);
                        
                    }
                    else{//merge right (still left)
                        Two4Node* y = curr -> child[t];
                        Two4Node* z = curr -> child[j];
                        return merge(y, curr, z, t, key);
                        
                    }
                    
                }
                
            }//end of if
            else if (curr -> child[j] -> numkeys >= 2) nodeDelete(curr -> child[j], key);
            else if (curr -> child[t] -> numkeys >= 2){
                if (t == j + 1)return rotateLeft(curr -> child[j], curr, curr -> child[t], j, key);//rotate left
                else return rotateRight(curr -> child[t], curr, curr -> child[j], t, key);//rotate right
            }
            else{
                if (t == j + 1){
                    Two4Node* newy = curr -> child[j];
                    Two4Node* newz = curr -> child[t];
                    return merge(newy, curr, newz, j, key);
                }
                else{
                    Two4Node* newy = curr -> child[t];
                    Two4Node* newz = curr -> child[j];
                    return merge(newy, curr, newz, t, key);
                    
                }
                
            }
            return this -> root;
        }

        Two4Node* rotateLeft(Two4Node* left, Two4Node* parent, Two4Node* right, int indl, K key){
            //move parent key into left
            left -> keys[1] = parent -> keys[indl];
            left -> values[1] = parent -> values[indl];
            //increase numkeys and sub accordingly
            left -> numkeys++;
            left -> sub++;
            //move right key into parent
            parent -> keys[indl] = right -> keys[0];
            parent -> values[indl] = right -> values[0];
            //adjust right's numkeys and sub
            right -> numkeys--;
            right -> sub--;
            //move remaining right keys left 1 to fill places
            for (int i = 0; i < right -> numkeys; i++) {right -> keys[i] = right -> keys[i + 1]; right -> values[i] = right -> values[i + 1];}

            if (!(right -> leaf)){//if right has children
                //move first right child over to left
                left -> child[left -> numkeys] = right -> child[0];
                //adjust subs (children)
                if (!(right -> child[0] -> leaf)){
                    left -> sub += left -> child[left -> numkeys] -> sub;
                    right -> sub -= right -> child[0] -> sub;
                }
                else {left -> sub += left -> child[left -> numkeys] -> numkeys; right -> sub -= right -> child[0] -> numkeys;}
                
                //move right children left one
                for (int i = 0; i < right -> numkeys + 1; i++) right -> child[i] = right -> child[i + 1];
            }//end of nested if
            right -> child[right -> numkeys + 1] = NULL;//get rid of last right child
            return nodeDelete(left, key);//delete from appropriate node
        }//end of rotateLeft

        Two4Node* rotateRight(Two4Node* left, Two4Node* parent, Two4Node* right, int indl, K key){
            int leftsz = left -> numkeys;//variable for numkeys
            //shift right node one
            right -> keys[1] = right -> keys[0];
            right -> values[1] = right -> values[0];
            //move parent value into right keys[0]
            right -> keys[0] = parent -> keys[indl];
            right -> values[0] = parent -> values[indl];
            right -> numkeys = 2;
            //move last left key into parent
            parent -> keys[indl] = left -> keys[leftsz - 1];
            parent -> values[indl] = left -> values[leftsz - 1];
            left -> numkeys--;//change left size
            leftsz--;//change var to match

            if (!(right -> leaf)){//if it has children
                //shift right's children down
                for (int i = right -> numkeys; i > 0; i--) right -> child[i] = right -> child[i - 1];
                //right first child should move over from left
                right -> child[0] = left -> child[leftsz + 1];//var change accounted for
                if (!(right -> child[0] ->leaf)){
                    right -> sub += right -> child[0] -> sub;//increase right sub and decrease left (for children)
                    left -> sub -= left -> child[leftsz + 1] -> sub;
                }
                else{right -> sub += right -> child[0] -> numkeys; left -> sub -= left -> child[leftsz + 1] -> numkeys;}
            }//end of nested if
            //increase right sub and decrease left (for keys)
            right -> sub++;
            left -> sub--;
            //make place where child used to be empty
            left -> child[leftsz + 1] = NULL;
            return nodeDelete(right, key);//delete from correct node
        }//end of rotateRight

        Two4Node* merge(Two4Node* left, Two4Node* parent, Two4Node* right, int indl, K key){

            int leftsz = left -> numkeys, rightsz = right -> numkeys, indr = indl + 1;
            //setting keys[1]
            left -> keys[1] = parent -> keys[indl];
            left -> values[1] = parent -> values[indl];
            parent -> numkeys--;//parent's numkeys decreases
            //set keys[2] to be right child's first key
            left -> keys[2] = right -> keys[0];
            left -> values[2] = right -> values[0];
            //move over children from right node
            if (!(left -> leaf))for (int i = 0; i <= 1; i++) {left -> child[i + 2] = right -> child[i];}
            //merged node has 3 keys and new sub
            left -> numkeys = 3;
            left -> sub = left -> sub + right -> sub + 1;
            //go through and shift parent's keys
            for (int i = indl; i < parent -> numkeys; i++) {parent -> keys[i] = parent -> keys[i + 1];parent -> values[i] = parent -> values[i + 1];}
            //shift parent's children
            for (int i = indl + 1; i <= parent -> numkeys; i++)parent -> child[i] = parent -> child[i + 1];
            //free(right);//free right node
            parent -> child[parent -> numkeys + 1] = NULL;
            //if there are no more keys left, pass root to child
            if (parent -> numkeys == 0){this -> root = left;}
            //recursively delete from new child
            return nodeDelete(left, key);
        }//end of merge

        V * nodeSearch(K key, Two4Node* curr){
            int i = 0;
            while(i < curr -> numkeys && curr -> keys[i] < key) i++;

            if (curr -> leaf && i == curr ->numkeys ) return NULL;
            else if (curr -> leaf && key != curr -> keys[i]) return NULL;
            
            if (i < curr -> numkeys && curr -> keys[i] == key) return &(curr -> values[i]);
            else return nodeSearch(key, curr -> child[i]);

        }//end of nodeSearch

        K nodeSelect(int pos, Two4Node* curr){
            
            int i = 0, pl = 0, last, newpos;
            if (curr -> leaf)return curr -> keys[pos - 1];

            while (!(curr -> leaf)){
                last = pl;
                if (i < curr -> numkeys) pl += curr -> child[i] -> sub + 1;
                else if (i == curr -> numkeys) pl += curr -> child[i] -> sub;
                if (pl >= pos) break;
                i++;
            }
            if (pl == 0) pl++;
            if (i > 0) newpos = pos - last;
            else newpos = pos;
            if (pos == pl && i < curr -> numkeys ) return curr -> keys[i];
            else return nodeSelect(newpos, curr -> child[i]);
        }

        int nodeRank(K key, Two4Node* curr){
            int i = 0, r = 0;
            
            while (i < curr -> numkeys && key > curr -> keys[i]) {if (!(curr -> leaf)) r += curr -> child[i] ->sub + 1; else r++; i++;}
            
            if (i < curr -> numkeys && key == curr -> keys[i]) {
                
                if (!(curr -> leaf)) return r + curr -> child[i] -> sub + 1;
                else return i + 1;
            }
            else if (curr -> leaf) return 0;
            else return r + nodeRank(key, curr -> child[i]);
        }
        
        void preorderNode(Two4Node* curr){
            if (curr == NULL) return;
            if (curr -> leaf){
                for (int i = 0; i < curr -> numkeys; i++)cout << curr -> keys[i]  << " ";
                return;
            }
            else{
                for (int i = 0; i < curr -> numkeys; i++) cout << curr -> keys[i] << " ";

                for (int i = 0; i <= curr -> numkeys; i++) preorderNode(curr -> child[i]);
            }
            
        }

        void inorderNode(Two4Node* curr){
            if (curr -> leaf){
                for (int i = 0; i < curr -> numkeys; i++)cout << curr -> keys[i]  << " ";
                return;
            }
            else{
                for (int i = 0; i < curr -> numkeys; i++){
                    inorderNode(curr -> child[i]);
                    cout <<curr -> keys[i]  << " ";
                 
                }
                inorderNode(curr -> child[curr -> numkeys]); 
            }
        }

        void postorderNode(Two4Node* curr){
            if (curr == NULL) return;
            if (curr -> leaf){
                for (int i = 0; i < curr -> numkeys; i++)cout << curr -> keys[i]  << " ";
                return;
            }
            else{
                for (int i = 0; i <= curr -> numkeys; i++) postorderNode(curr -> child[i]);

                for (int i = 0; i < curr -> numkeys; i++) cout << curr -> keys[i] << " ";
            }
            
            
        }

        K succDel(K key, Two4Node* curr, Two4Node* ch) {return nodeSelect(1, ch);}

        K predDel(K key, Two4Node* curr, Two4Node* ch){return nodeSelect(ch -> sub, ch);}

        K succ(K key, Two4Node* curr){int crank = nodeRank(key, curr); if (crank < curr -> sub) return nodeSelect(crank + 1, curr); else return 0;}

        K pred(K key, Two4Node* curr){int crank = nodeRank(key, curr); if (crank > 1) return nodeSelect(crank - 1, curr); else return 0;}

    public:

        Two4Tree(){
            root = NULL;
            sz = 0;
        }

        Two4Tree(K k[], V v[], int s){
            root = NULL;
            sz = 0;
            for (int i = 0; i < s; i++) root = insert(root, k[i], v[i]);
        }

        Two4Tree(const Two4Tree& right){
            if (right.root != NULL) root = copy(right.root);
            
            sz = right.sz;
        }

        Two4Tree& operator= (const Two4Tree& right){
            if (right.root != NULL) root = copy(right.root);
            sz = right.sz;
            
            return *this;
        }

        ~Two4Tree(){
            if (root != NULL)clearMem(root);
            root = NULL;
            sz = 0;

        }

        void insert(K k, V v){root = insert(root, k, v);}

        V * search(K k){return nodeSearch(k, root);}

        int size(){return root -> sub;}
        
        K getRoot(){return root ->keys[0];}
        
        int test(){return subTreeSize(root, 0);}

        int rank(K k){if (nodeSearch(k, root) == NULL) return 0; else return nodeRank(k, root);}

        K select(int pos){if(root -> sub < pos) return 0; else return nodeSelect(pos, root);}

        void preorder(){preorderNode(root); cout << endl;}
        
        void inorder(){inorderNode(root); cout << endl;}

        void postorder(){postorderNode(root); cout << endl;}

        K successor(K k){return succ(k, root);}

        K predecessor(K k){return pred(k, root);}

        int remove (K k){
            if (nodeSearch(k, root) == NULL) return 0;
            else{
                root = nodeDelete(root, k);
                if (root -> sub == 0) root = NULL;
                return 1;
            }
        }

}; 