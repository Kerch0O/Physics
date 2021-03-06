#include "functions.h"


void rigidBody::evaluateLinks() {

	//Update particle positions based off of links

	for (auto& x : links) {
		x.pullParticles(linkedParticles);
	}

}


void rigidBody::initialiseLinks() {
	//make links

	links.push_back(Link(linkedParticles, 0, 1, 2.0f)); //Horizontals & Verticals
	links.push_back(Link(linkedParticles, 1, 2, 2.0f));
	links.push_back(Link(linkedParticles, 2, 3, 2.0f));
	links.push_back(Link(linkedParticles, 3, 0, 2.0f));
	links.push_back(Link(linkedParticles, 0, 2, 2.0f)); //Diags
	links.push_back(Link(linkedParticles, 1, 3, 2.0f));

}
void rigidBody::particleCollision(Particle* p) {

	//Find closest side using closest point checker
	if (p->rigidType != linkedParticles[0]->rigidType) {
		int closestP = 0;
		float currentD = pythag(linkedParticles[0]->rep.getPosition() - p->rep.getPosition());
		float closestD = currentD;

		for (int i = 1; i < linkedParticles.size(); i++) {
			currentD = pythag(linkedParticles[i]->rep.getPosition() - p->rep.getPosition());
			if (currentD < closestD) {
				closestP = i;
				closestD = currentD;
			}
		}

		int i1 = closestP - 1, i2 = closestP + 1, nClosest = -1;//Get points indexes opposite of the sides connected
		i1 = i1 < 0 ? 3 : i1;
		i2 = i2 > 3 ? 0 : i2; //Confine into boundaries
	

		//Closest of the two new points will be the other point of the line which is closest
		closestD = pythag(linkedParticles[i2]->rep.getPosition() - p->rep.getPosition());
		if (linkedParticles[i1] != p && pythag(linkedParticles[i1]->rep.getPosition() - p->rep.getPosition()) < closestD) {
			nClosest = i1;
		}
		else {
			nClosest = i2;
		}

		//Now that we have the closest and the next closest, we can now check for a collision between the points of a rectangle
		//where W = point diameter and L = position diff
		//Using line math to find the closest particle, then checking collision with it
		
		sf::Vector2f pointD = linkedParticles[nClosest]->rep.getPosition() - linkedParticles[closestP]->rep.getPosition();
		float m = pointD.y / pointD.x;
		float c = -m * linkedParticles[closestP]->rep.getPosition().x + linkedParticles[closestP]->rep.getPosition().y;
		float pm = -1.0f / m;
		float pc = -pm * p->rep.getPosition().x + p->rep.getPosition().y;//Perpendicular line connecting with the particle

		//Consolidate final position
		sf::Vector2f closestOnLine;

		if (m == 0) {
			closestOnLine.x = p->rep.getPosition().x;
			closestOnLine.y = linkedParticles[closestP]->rep.getPosition().y;
		}
		else if (isinf(m)) {
			closestOnLine.x = linkedParticles[closestP]->rep.getPosition().x;
			closestOnLine.y = p->rep.getPosition().y;
		}
		else {

			closestOnLine.x = (pc - c) / (m - pm);
			closestOnLine.y = m * closestOnLine.x + c;//y = mx + c
		}


		sf::Vector2f d = p->rep.getPosition() - closestOnLine;

		

		

		if (p->rep.getRadius() >= pythag(d)) {
			//Collision
			//Apply standard verlet collision with mass ratio

			float totalMass = fullMass + p->mass;
				

			float movingDistance = p->rep.getRadius() - pythag(d);

			d /= pythag(d); //Turn into unit vector

			if(p->movable)
				p->rep.move(d * movingDistance * (p->mass / totalMass));
			if(linkedParticles[closestP]->movable)
				linkedParticles[closestP]->rep.move(-d * movingDistance * (fullMass / totalMass * 0.5f));
			if(linkedParticles[nClosest]->movable)
				linkedParticles[nClosest]->rep.move(-d * movingDistance * (fullMass / totalMass * 0.5f));	
		}
	}
	
}


rigidBody::rigidBody() 
: linkedParticles(4, nullptr), fullMass(0.0f)
{}

