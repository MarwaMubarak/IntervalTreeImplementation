#include <iostream>
using namespace std;
// interval data
struct Interval{
    int low=1e9;
    int high=1e9;
};
class IntervalTree{
    IntervalTree *left;
    IntervalTree *right;
    int mx;
    Interval interval;
public:
    IntervalTree(const Interval &interval) : interval(interval) {
        left = right = nullptr;
        mx = interval.high;
    }

    IntervalTree(){
        interval.low = interval.high= 0;
        right = left = nullptr;
        mx = 0;
    };
    IntervalTree *insertInterval(IntervalTree* node,  Interval interval)
    {
        // tree is empty create new node as a root
        if(node == nullptr)
        {
            return new IntervalTree(interval);
        }
        int currentLow = node->interval.low;
        // if low in new node less than low in current node so new interval goes to left subTree
        if(interval.low < currentLow)
            node->left = insertInterval(node->left, interval);
            // else right subTree
        else if(interval.low > currentLow)
            node->right = insertInterval(node->right, interval);
        else
        {
            // if interval.low equals current.low check the high value
            if(interval.high > node->interval.high)
                node->right = insertInterval(node->right, interval);
            else
                node->left = insertInterval(node->left, interval);
        }
        // update mx of current node
        if (node->mx < interval.high)
            node->mx = interval.high;
        return node;
    }
    IntervalTree *deleteIntervalNode(IntervalTree * node, Interval interval)
    {
        // interval not found
        if (node == nullptr)
            return node;
        //  if low in new node less than low in current node so new interval goes to left subTree
        if (interval.low < node->interval.low)
            node->left = deleteIntervalNode(node->left, interval);
            // if low in new node greater than low in current node so new interval goes to right subTree
        else if (interval.low > node->interval.low)
            node->right = deleteIntervalNode(node->right, interval);
        else {
            // if equal check the high value
            if(interval.high < node->interval.high)
                node->left = deleteIntervalNode(node->left, interval);
            else if (interval.high > node->interval.high)
                node->right = deleteIntervalNode(node->right, interval);
            else
            {
                IntervalTree* tempRight = node->right;
                IntervalTree* tempLeft = node->left;
                // If the node is a leaf, replace it with null
                if (tempLeft == nullptr && tempRight == nullptr) {
                    return nullptr;
                }
                    // if it has right child ; link parent with right and delete the node
                else if (tempLeft == nullptr) {
                    delete node;
                    return tempRight;
                }
                    // if it has left child ; link parent with left and delete the node
                else if (tempRight == nullptr) {
                    delete node;
                    return tempLeft;
                }
                // if it has 2 children
                IntervalTree *rightMostNode = node->right;
                // Get the left most node in the right tree
                while (rightMostNode->left != nullptr)
                    rightMostNode = rightMostNode->left;
                // Copy the interval in right to the node to be deleted
                node->interval = rightMostNode->interval;
                // remove right
                node->right = deleteIntervalNode(node->right,(node->interval));
            }
        }
        // update the max of the nodes of all the tree
        node->mx=node->interval.high;
        if(node->right!= nullptr)
            node->mx= max(node->right->mx,node->mx);
        if(node->left!= nullptr)
            node->mx= max(node->left->mx,node->mx);
        return node;
    }
    // traverse the tree
    void inorder(IntervalTree *root)
    {
        if (root == nullptr) return;
        inorder(root->left);
        cout << "[" << root->interval.low << ", " << root->interval.high << "]"
             << " mx = " << root->mx << endl;

        inorder(root->right);
    }
    // overlap condition
    bool isOverLap(Interval query, Interval tree)
    {
        if(query.low <= tree.high && tree.low <= query.high)
            return true;
        return false;
    }

    IntervalTree*  insertDeletedNodes(IntervalTree * root , Interval deletedIntervals[],int cnt){
        // loop over all the deleted intervals and insert all again
        for (int i = 0; i < cnt; ++i) {
            root = root->insertInterval(root,deletedIntervals[i]);
        }
        return root;
    }
    IntervalTree* search(Interval query,IntervalTree * root)
    {
        int cnt = 0;// represent the current size of the deletedIntervals
        Interval * deletedIntervals = new Interval[10000]; // the deleted Intervals

        int f=cnt; // save the current size of the deleted Intervals
        SearchInterval(query,root, root,cnt,deletedIntervals); // search about one interval
        // while the size of the deletedIntervals change there may be another interval overlap
        while (f<cnt) {
            // delete the interval we found
            root=deleteIntervalNode(root,deletedIntervals[cnt-1]);
            //if the root is null (we deleted all the intervals)
            if(root== nullptr)
                break;
            f=cnt; // save the current size of the deleted Intervals and if it changes it means that there is an overlap interval
            SearchInterval(query,root, root,cnt,deletedIntervals);
        }
        // there is no overlap
        if(cnt == 0)
            cout<<"Not found overlap\n";
        else{
            // insert all deleted intervals again
            root=insertDeletedNodes(root,deletedIntervals,cnt);
        }
        return root;

    }
    void  SearchInterval(Interval query ,IntervalTree *root,IntervalTree *node,int & cnt,Interval  deletedIntervals[]){
        // if the root is null return (empty)
        if(node== nullptr)
            return ;
        // check the overlap condition
        if(isOverLap(query,node->interval)){
            // print it
            cout<<'['<<query.low<<','<<query.high<<']'<<" OverLap with "<<'['<<node->interval.low<<','<<node->interval.high<<"]"<<endl;
            // put the interval to delete it and increase the cnt then return
            deletedIntervals[cnt]=node->interval;
            cnt++;
            return ;
        }
        // keep search
        // if (query.low > root->left->mx) is true it can never overlap with left subTree

        if(node->left!= nullptr && query.low>node->left->mx){
            SearchInterval(query,root,node->right,cnt,deletedIntervals);
        }else {
            SearchInterval(query,root,node->right,cnt,deletedIntervals);
            SearchInterval(query,root,node->left,cnt,deletedIntervals);

        }

    }
};
int main() {

    Interval intervals1[]={{1,14},{2,15},{3,16},{50,60},{55,70}};
    IntervalTree* intervalTree1= nullptr;
    int n1= sizeof (intervals1) / sizeof (intervals1[0]);
    for (int i = 0; i < n1; ++i) {
        intervalTree1=intervalTree1->insertInterval(intervalTree1, intervals1[i]);
    }
    cout<<"Test1"<<endl;
    intervalTree1=intervalTree1->search({1, 15},intervalTree1); // overLaps with [1,14], [2,15], [3,16]
    cout<<"\nTest2"<<endl;
    intervalTree1=intervalTree1->search({45, 46},intervalTree1); // the overLap condition is false
    cout<<"\nTest3"<<endl;
    intervalTree1=intervalTree1->search({45, 100},intervalTree1); // OverLap with [50,60], [55,70]
    cout<<"\nTest4"<<endl;
    intervalTree1 = intervalTree1->search({1, 100},intervalTree1);// OverLap with all intervals in the tree
    //intervalTree1->inorder(intervalTree1);


    Interval intervals2[]={{50, 51}, {3, 30}, {4, 40}, {80, 88}, {-100, 100}};
    IntervalTree* intervalTree2= nullptr;
    int n2= sizeof (intervals2) / sizeof (intervals2[0]);
    for (int i = 0; i < n2; ++i) {
        intervalTree2=intervalTree2->insertInterval(intervalTree2, intervals2[i]);
    }
    cout<<"\nTest5"<<endl;
    intervalTree2=intervalTree2->search({1, 15},intervalTree2);//  OverLap with [3,30],  [4,40], [-100,100]
    cout<<"\nTest6"<<endl;
    intervalTree2=intervalTree2->search({45, 46},intervalTree2);// OverLap with [-100,100]
    cout<<"\nTest7"<<endl;
    intervalTree2=intervalTree2->search({45, 100},intervalTree2);// OverLap with [50,51], [80,88],[-100,100]
    cout<<"\nTest8"<<endl;
    intervalTree2=intervalTree2->search({1, 100},intervalTree2);// OverLap with all intervals in the tree
    cout<<"\nTest9"<<endl;
    intervalTree2=intervalTree2->search({-200, -201},intervalTree2);//  overLap condition is false
    //intervalTree2->inorder(intervalTree2);



    Interval intervals3[]={{2, 51}, {-10, 0}, {3, 3}, {9, 19}, {-10,2 }};
    IntervalTree* intervalTree3= nullptr;
    int n3= sizeof (intervals3) / sizeof (intervals3[0]);
    for (int i = 0; i < n3; ++i) {
        intervalTree3=intervalTree3->insertInterval(intervalTree3, intervals3[i]);
    }
    cout<<"\nTest10"<<endl;
    intervalTree3=intervalTree3->search({-1, 4},intervalTree3);// OverLap with [2,51],  [3,3], [-10,0], [-10,2]
    cout<<"\nTest11"<<endl;
    intervalTree3=intervalTree3->search({3, 3},intervalTree3);//OverLap with [2,51], [3,3]
    cout<<"\nTest12"<<endl;
    intervalTree3=intervalTree3->search({4, 100},intervalTree3);// OverLap with [9,19], [2,51]
    cout<<"\nTest13"<<endl;
    intervalTree3=intervalTree3->search({-10, 1},intervalTree3);// OverLap with [-10,0], [-10,2]
    cout<<"\nTest14"<<endl;
    intervalTree3=intervalTree3->search({-200, 100},intervalTree3);// OverLap with all intervals in the tree
    cout<<"\nTest15"<<endl;
    intervalTree3=intervalTree3->search({-200, -150},intervalTree3);//  overLap condition is false
    //intervalTree3->inorder(intervalTree3);
    Interval intervals4[]={{11, 14}, {2, 4}, {3, 16}, {30, 29}, {7, 55}, {40, 46}, {21, 74}};

    int n4 = sizeof(intervals4) / sizeof(intervals4[0]);
    IntervalTree *intervalTree4 = NULL;
    for (int i = 0; i < n4; i++)
        intervalTree4 = intervalTree4->insertInterval(intervalTree4, intervals4[i]);
    cout<<"\nTest16"<<endl;
    intervalTree4=intervalTree4->search({3, 17}, intervalTree4);// OverLap with [11,14], [2,4],[3,16],[7,55]

    cout<<"\nTest17"<<endl;
    intervalTree4=intervalTree4->search({-1, 0}, intervalTree4);//  overLap condition is false

    cout<<"\nTest18"<<endl;
    intervalTree4=intervalTree4->search({3, 8}, intervalTree4);//OverLap with [2,4],[3,16],[7,55]

    cout<<"\nTest19"<<endl;
    intervalTree4=intervalTree4->search({1, 50}, intervalTree4);// OverLap with all intervals in the tree

    cout<<"\nTest20"<<endl;
    intervalTree4=intervalTree4->search({100, 101}, intervalTree4);//  overLap condition is false

    Interval intervals5[]={{1, 14}, {2, 4}, {3, 16}, {30, 29}, {7, 55}, {40, 46}, {21, 74},{-100,3},{-2,5},{0,0}};

    int n5 = sizeof(intervals5) / sizeof(intervals5[0]);
    IntervalTree *intervalTree5 = NULL;
    for (int i = 0; i < n5; i++)
        intervalTree5 = intervalTree5->insertInterval(intervalTree5, intervals5[i]);
    cout<<"\nTest21"<<endl;
    intervalTree5=intervalTree5->search({-1, 1}, intervalTree5);//OverLap with [1,14], [-100,3],[-2,5],[0,0]

    cout<<"\nTest22"<<endl;
    intervalTree5=intervalTree5->search({-10, 5}, intervalTree5);//OverLap with [2,4],[3,16],[1,14],[-100,3],[-2,5],[0,0]

    cout<<"\nTest23"<<endl;
    intervalTree5=intervalTree5->search({-5, 8}, intervalTree5);//OverLap with [7,55],[2,4],[3,16],[1,14],[-100,3],[-2,5],[0,0]

    cout<<"\nTest24"<<endl;
    intervalTree5= intervalTree5->search({1, 33}, intervalTree5);//OverLap with [21,74],[30,29],[7,55],[2,4],[3,16],[1,14], [-100,3],[-2,5]

    cout<<"\nTest25"<<endl;
    intervalTree5= intervalTree5->search({0, 1}, intervalTree5);//OverLap with [0,0],[1,14],[-100,3],[-2,5]

    Interval intervals6[]={{10, 20}, {-4, 4}, {300, 333}, {70, 79}, {3, 55}, {40, 66}, {1, 1},{2,2}};

    int n6 = sizeof(intervals6) / sizeof(intervals6[0]);
    IntervalTree *intervalTree6 = NULL;
    for (int i = 0; i < n6; i++)
        intervalTree6 = intervalTree6->insertInterval(intervalTree6, intervals6[i]);
    cout<<"\nTest26"<<endl;
    intervalTree6=intervalTree6->search({1, 1}, intervalTree6);// OverLap with [-4,4],[1,1]

    cout<<"\nTest27"<<endl;
    intervalTree6=intervalTree6->search({-11, 2}, intervalTree6);// OverLap with [2,2],[-4,4],[1,1]

    cout<<"\nTest28"<<endl;
    intervalTree6=intervalTree6->search({9,9}, intervalTree6);//OverLap with [3,55]

    cout<<"\nTest29"<<endl;
    intervalTree6=intervalTree6->search({11, 33}, intervalTree6);//OverLap with [10,20],[3,55]

    cout<<"\nTest30"<<endl;
    intervalTree6=intervalTree6->search({0, 13}, intervalTree6);// OverLap with [2,2],[3,55],[10,20],[-4,4],[1,1]





    return 0;
}

