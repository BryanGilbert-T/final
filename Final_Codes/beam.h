#ifndef beam_h
#define beam_h

typedef struct _Beam{
	Point coord;
	int speed;
	int damage;
	ALLEGRO_BITMAP * image;
} Beam;

typedef struct _BeamNode {
	Beam beam;
	struct _BeamNode* next;
} BeamNode;


Beam createBeam(char* path, int speed, int damage, Point playerCoord);
BeamNode* createBeamNode(void);
void updateBeamNode(BeamNode* beamNode, Point cam, int speed);
void drawBeamNode(BeamNode * beamNode, Point cam);
void deleteBeamNode(BeamNode* beamNode);
void insertBeamNode(BeamNode* head, Beam beam);


#endif