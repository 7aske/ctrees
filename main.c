#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <unistd.h>

#define BUF_S 16
#define TREE_FILE "tree.txt"

struct tnode {
	int data;
	struct tnode* lchild;
	struct tnode* rchild;
};

/*---------FLAGS------------*/
/**/int MEM_FLAG = 0;     /**/
/**/int FIND_FLAG = 0;    /**/
/**/int SLEEP_FLAG = 0;   /**/
/**/int SWAP_FLAG = 0;    /**/
/**/int DEL_FLAG = 0;     /**/
/**/int CREATE_FLAG = 0;  /**/
/**/int COUNT_FLAG = 0;   /**/
/**/int TRAV_FLAG = 0;    /**/
/**/int HELP_FLAG = 0;    /**/
/*--------------------------*/
void printHeader() {
#ifdef __linux__
	system("clear -x");
#elif _WIN32
	system("cls");
#else
#endif
	printf("\033[1;37m%-4s%-4s%-4s%-4s%-4s%-4s%-4s%-4s\n\033[00m", "0", "1", "2", "3", "4", "5", "6", "7");
	printf("\033[1;37m--------------------------------\n\033[00m");
}

void printTree(struct tnode* root, int tab, int color) {
	if (root == NULL) {
		for (int i = 0; i < tab; i++)
			printf("%4s", "");
		printf(" \n");
		return;
	}
	printTree(root->lchild, tab + 1, 1);
	for (int i = 0; i < tab; i++)
		printf("%4s", "");
	if (MEM_FLAG == 1) {
		if (color == 1) {
			printf("(0x%x)\033[1;32m%d\033[00m\n", root, root->data);
		} else if (color == 2) {
			printf("(0x%x)\033[1;31m%d\033[00m\n", root, root->data);
		} else {
			printf("(0x%x)\033[1;33m%d\033[00m\n", root, root->data);
		}
	} else {
		if (color == 1) {
			printf("\033[1;32m%d\033[00m\n", root->data);
		} else if (color == 2) {
			printf("\033[1;31m%d\033[00m\n", root->data);
		} else {
			printf("\033[1;33m%d\033[00m\n", root->data);
		}
	}
	printTree(root->rchild, tab + 1, 2);
}

void printTreeDefault(struct tnode* root, int tab) {
	if (root == NULL)
		return;
	printTreeDefault(root->rchild, tab + 1);
	for (int i = 0; i < tab; i++)
		printf("%4s", "");
	printf("%d\n", root->data);
	printTreeDefault(root->lchild, tab + 1);
}

int count(struct tnode* p) {
	if (p == NULL)
		return 0;
	else if (p->lchild == NULL && p->rchild == NULL)
		return 1;
	else
		return 1 + count(p->lchild) + count(p->rchild);
}


struct tnode* search(struct tnode* p, int key) {
	struct tnode* temp;
	temp = p;
	while (temp != NULL) {
		if (temp->data == key)
			return temp;
		else if (temp->data > key)
			temp = temp->lchild;
		else
			temp = temp->rchild;
	}
	return NULL;
}

struct tnode* getptr(struct tnode* p, int key, struct tnode** y) {
	struct tnode* temp;
	if (p == NULL)
		return NULL;
	temp = p;
	*y = NULL;
	while (temp != NULL) {
		if (temp->data == key)
			return temp;
		else {
			*y = temp;
			if (temp->data > key)
				temp = temp->lchild;
			else
				temp = temp->rchild;
		}
	}
	return NULL;
}

struct tnode* delete(struct tnode* p, int val) {
	struct tnode* x, * y, * temp;
	x = getptr(p, val, &y);
	if (x == NULL) {
		fprintf(stderr, "Node doesnt exits \n");
		return p;
	} else { /* Ako se izbacuje koren */
		if (x == p) {
			temp = x->lchild;
			y = x->rchild;
			p = temp;
			while (temp->rchild != NULL)
				temp = temp->rchild;
			temp->rchild = y;
			free(x);
			return p;
		}
		if (x->lchild != NULL && x->rchild != NULL) {
			if (y->lchild == x) { /* Ako se izbacuje cvor sa dva cvor – deteta */
				temp = x->lchild;
				y->lchild = x->lchild;
				while (temp->rchild != NULL)
					temp = temp->rchild;
				temp->rchild = x->rchild;
				x->lchild = NULL;
				x->rchild = NULL;
			} else {
				temp = x->rchild;
				y->rchild = x->rchild;
				while (temp->lchild != NULL)
					temp = temp->lchild;
				temp->lchild = x->lchild;
				x->lchild = NULL;
				x->rchild = NULL;
			}
			free(x);
			return p;
		}
		if (x->lchild == NULL && x->rchild != NULL) {
			if (y->lchild == x) /* Ako se izbacuje cvor koji ima jedan cvor – dete */
				y->lchild = x->rchild;
			else
				y->rchild = x->rchild;
			x->rchild = NULL;
			free(x);
			return p;
		} else if (x->lchild != NULL && x->rchild == NULL) {
			if (y->lchild == x) /* Ako se izbacuje cvor koji ima jedan cvor – dete */
				y->lchild = x->lchild;
			else
				y->rchild = x->lchild;
			x->lchild = NULL;
			free(x);
			return p;
		} else { /* Ako se izbacuje list */
			if (y->lchild == x)
				y->lchild = NULL;
			else
				y->rchild = NULL;
			free(x);
			return p;
		}
	}
}

struct tnode* insert(struct tnode* p, int val) {
	struct tnode* temp1 = NULL;
	struct tnode* temp2 = NULL;
	if (p == NULL) {
		p = (struct tnode*) malloc(sizeof(struct tnode));
		p->data = val;
		p->lchild = p->rchild = NULL;
	} else {
		temp1 = p;
		while (temp1 != NULL) {
			temp2 = temp1;          /* prolazimo kroz stablo sve dok ne dodjemo do pokazivaca na onaj cvor ciji*/
			if (temp1->data > val)  /*naslednik ce biti novokreirani cvor */
				temp1 = temp1->lchild;
			else
				temp1 = temp1->rchild;
		}
		if (temp2->data > val) {
			temp2->lchild = (struct tnode*) malloc(sizeof(struct tnode));
			temp2 = temp2->lchild;/*ubacujemo novokreirani cvor kao levi naslednik*/
			temp2->data = val;
			temp2->lchild = NULL;
			temp2->rchild = NULL;

		} else {
			temp2->rchild = (struct tnode*) malloc(sizeof(struct tnode));
			temp2 = temp2->rchild;/*ubacujemo novokreirani cvor kao desni naslednik*/
			temp2->data = val;
			temp2->lchild = NULL;
			temp2->rchild = NULL;
		}
	}
	return p;
}

struct tnode* swaptree(struct tnode* p) {
	struct tnode* temp1 = NULL, * temp2 = NULL;
	if (p != NULL) {
		temp1 = swaptree(p->lchild);
		temp2 = swaptree(p->rchild);
		p->rchild = temp1;
		p->lchild = temp2;
	}
	return p;
}


void inorder(struct tnode* p) {
	if (p != NULL) {
		inorder(p->lchild);
		printf("%d ", p->data);
		inorder(p->rchild);
	}
}

void preorder(struct tnode* p) {
	if (p != NULL) {
		printf("%d ", p->data);
		preorder(p->lchild);
		preorder(p->rchild);
	}
}

void postorder(struct tnode* p) {
	if (p != NULL) {
		postorder(p->lchild);
		postorder(p->rchild);
		printf("%d ", p->data);
	}
}

void printHelp() {
	printf("usage: trees [--key=val]... [options]...\n");
	printf("\t%-16s%10s\n", "--find=val", "search for a node with a value <val>");
	printf("\t%-16s%10s\n", "--del=val", "delete a node with a value <val>");
	printf("\t%-16s%10s\n", "--swap=val", "swap children of a node with a value <val>");
	printf("\t%-16s%10s\n", "--help, -h", "show this message");
	printf("toggles: \n");
	printf("\t%-16s%10s\n", "-C", "start interactive tree-create mode");
	printf("\t%-16s%10s\n", "-c", "count tree nodes");
	printf("\t%-16s%10s\n", "-T", "list traverse types");
	printf("\t%-16s%10s\n", "-s", "sleep for a second after printing a node");
	printf("\t%-16s%10s\n", "-m", "show node memory locations");
}

int main(int argc, char* argv[]) {
	struct tnode* root = NULL;
	int search_val = 0;
	char buf[BUF_S];
	memset(buf, 0, sizeof(buf));

	while (*argv != NULL) {
		if (strncmp(*argv, "--help", 6) == 0 || strncmp(*argv, "-h", 2) == 0) {
			HELP_FLAG = 1;
			break;
		} else if (strncmp(*argv, "--find=", 9) == 0) {
			FIND_FLAG = 1;
			char* argvptr = strchr(*argv, '=');
			argvptr++;
			strncpy(buf, argvptr, strlen(argvptr));
			search_val = strtol(buf, NULL, 10);
			memset(buf, 0, sizeof(buf));
		} else if (strncmp(*argv, "--swap=", 7) == 0) {
			SWAP_FLAG = 1;
			char* argvptr = strchr(*argv, '=');
			argvptr++;
			strncpy(buf, argvptr, strlen(argvptr));
			search_val = strtol(buf, NULL, 10);
			memset(buf, 0, sizeof(buf));
		} else if (strncmp(*argv, "--del=", 6) == 0) {
			DEL_FLAG = 1;
			char* argvptr = strchr(*argv, '=');
			argvptr++;
			strncpy(buf, argvptr, strlen(argvptr));
			search_val = strtol(buf, NULL, 10);
			memset(buf, 0, sizeof(buf));
		} else if (strncmp(*argv, "-m", 2) == 0) {
			MEM_FLAG = 1;
		} else if (strncmp(*argv, "-s", 2) == 0) {
			SLEEP_FLAG = 1;
		} else if (strncmp(*argv, "-c", 2) == 0) {
			COUNT_FLAG = 1;
		} else if (strncmp(*argv, "-C", 2) == 0) {
			CREATE_FLAG = 1;
		} else if (strncmp(*argv, "-T", 2) == 0) {
			TRAV_FLAG = 1;
		}
		argv++;
	}

	if (HELP_FLAG) {
		printHelp();
		exit(0);
	} else if (CREATE_FLAG) {
		int num_nodes = 0;
		printf("Number of nodes: ");
		fgets(buf, sizeof(buf), stdin);
		num_nodes = strtol(buf, NULL, 10);
		if (num_nodes > 20) {
			errno = EINVAL;
			perror("Too many nodes");
			exit(1);
		}
		while (num_nodes > 0) {
			printf("Node %d: ", num_nodes);
			if (fgets(buf, sizeof(buf), stdin)) {
				root = insert(root, (int) strtol(buf, NULL, 10));
				memset(buf, 0, sizeof(buf));
				num_nodes--;

				printHeader();
				printTree(root, 0, 0);
			}
		}
	} else {
		FILE* fp = fopen(TREE_FILE, "r");
		if (fp == 0) {
			errno = ENOENT;
			perror("Error opening tree file");
			exit(1);
		}
		while (fgets(buf, sizeof(buf), fp) != NULL) {
			buf[strlen(buf) - 1] = '\0';
			root = insert(root, (int) strtol(buf, NULL, 10));
			memset(buf, 0, sizeof(buf));

			printHeader();
			printTree(root, 0, 0);
			if (SLEEP_FLAG == 1)
				sleep(1);
		}
	}


	if (FIND_FLAG) {
		struct tnode* search_node = search(root, search_val);
		if (search_node != NULL) {
			printf("Node found: (0x%x)%d\n", search_node, search_node->data);
		} else {
			printf("Node not found\n");
		}
	} else if (SWAP_FLAG) {
		struct tnode* swap_node = search(root, search_val);
		if (swap_node != NULL) {
			swaptree(swap_node);
			printHeader();
			printTree(root, 0, 0);
			printf("Swapped children of node: (0x%x)%d\n", swap_node, swap_node->data);
		} else {
			printf("Node not found\n");
		}
	} else if (DEL_FLAG) {
		delete(root, search_val);
		printHeader();
		printTree(root, 0, 0);
		printf("Deleted node with val:%d\n", search_val);
	}
	if (TRAV_FLAG) {
		printf("Inorder list   (LDR):\n");
		inorder(root);
		printf("\nPreorder list  (DLR):\n");
		preorder(root);
		printf("\nPostorder list (LRD):\n");
		postorder(root);
		printf("\n");

	}

	if (COUNT_FLAG) {
		printf("\n\033[1;35mNumber of nodes: %d\n", count(root));
	}

	return 0;
}
