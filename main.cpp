#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

struct rect{
    olc::vf2d pos;
    olc::vf2d size;
};

/*
l(t) = p + t * v (ray)
r(s) = q + s * w (considering the line segment where 0 <= s <= 1)

This function calculates the point where ray l intersects line segment r. Note that segment defined by l(t) does not
need to cross r(s), since it considers l to be an infinitely long line defined by point p and
direction vector v.

THe function returns true if and only if ray l intersects line segment s and the vector from p to the intersection
point is in the same direction as the ray.
*/
bool rayToSegmentIntersection(const olc::vf2d& p, const olc::vf2d& v, 
                              const olc::vf2d& q, const olc::vf2d& w, 
                              olc::vf2d& out_intersection){

    float det = v.x * w.y - v.y * w.x;
    
    const float epsilon = 1e-9;

    if(fabs(det) < epsilon){
        return false;
    }

    float dx = p.x - q.x;
    float dy = p.y - q.y;

    float s = (dy * v.x - dx * v.y) / det;
    
    if(s < 0 || s > 1)
        return false;

    out_intersection = q + (s * w);

    // Check the direction of the intersection point with respect to the ray
    olc::vf2d direction = out_intersection - p;

    float dotProduct =  v.dot(direction);

    return dotProduct >= 0;

}

/*
l(t) = p + t * v (ray segment)
r(s) = q + s * w (considering the line segment where 0 <= s <= 1)

This function calculates the point where ray segment l intersects line segment r.
l must cross r for the intersection to be detected.
*/
bool raySegmentToLineSegmentIntersection(const olc::vf2d& p, const olc::vf2d& v, 
                              const olc::vf2d& q, const olc::vf2d& w, 
                              olc::vf2d& out_intersection){

    // l(t) = p + t * v (ray segment)
    // r(s) = q + s * w (considering the line segment where 0 <= s <= 1)

    const float epsilon = 1e-9;
    float den = (v.y * w.x - v.x * w.y);
    if (fabs(den) < epsilon){
        return false;
    }

    float dx = q.x - p.x;
    float dy = q.y - p.y;

    float t = (dy * w.x - dx * w.y) / den;
    if (t < 0 || t > 1){
        return false;
    }

    float s = (dy * v.x - dx * v.y) / den;
    if (s < 0 || s > 1){
        return false;
    }

    out_intersection = p + t * v;

    return true;
}


/*
This function calculates the closest intersection point from `p` in direction `v` to
the sides of the bounding boxes in `rects`. 

Output parameters return the closest rectangle index in `rects` (`out_closest_rect_idx`), 
the closest intersection point (`out_closest_intersection`), 
and distance from `p` to the closest intersection point (`out_closest_intersection_dist`).

Returns true if an intersection is found.
*/
bool raycastCollision(const std::vector<rect>& rects, const olc::vf2d& p, const olc::vf2d& v, 
                      int& out_closest_rect_idx, 
                      olc::vf2d& out_closest_intersection, 
                      float& out_closest_intersection_dist){

    int i, closest = 0;
    float closest_d = std::numeric_limits<float>::infinity();
    olc::vf2d closest_intersection;

    auto dist = [](const olc::vf2d& u, const olc::vf2d& v){ return (u-v).mag(); };

    olc::vf2d intersection;

    for(auto& r : rects){
        float x, y, w, h;
        x = r.pos.x;  y = r.pos.y;
        w = r.size.x; h = r.size.y;

        std::vector<std::pair<olc::vf2d, olc::vf2d>> sides = {
            {{r.pos},{w, 0}},
            {{r.pos},{0, h}},
            {{x, y+h},{w, 0}},
            {{x+w, y},{0, h}}
        };

        for(auto& s : sides){
            if(rayToSegmentIntersection(p, v, s.first, s.second, intersection)){
                float d = dist(p, intersection);
                if(d < closest_d){
                    closest = i;
                    closest_d = d;
                    closest_intersection = intersection;
                }
            }
        }
        i++;
    }

    if(closest_d == std::numeric_limits<float>::infinity())
        return false;

    out_closest_rect_idx = closest;
    out_closest_intersection = closest_intersection;
    out_closest_intersection_dist = closest_d;

    return true;
}

class RayCastingCollisions : public olc::PixelGameEngine
{
public:
	RayCastingCollisions()
	{
		sAppName = "RayCasting Collisions!";
	}

	bool OnUserCreate() override
	{
		rects.push_back({{300.0f, 100.0f}, {20.0f, 20.0f}});
        rects.push_back({{290.0f, 140.0f}, {20.0f, 20.0f}});
        rects.push_back({{270.0f, 170.0f}, {20.0f, 20.0f}});

        p = {250.0f, 250.0f};

		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{

        if(GetKey(olc::Key::P).bPressed){
            p = GetMousePos();
        }

        auto& mouse = GetMousePos();
        
        v = mouse - p;

        Clear(olc::DARK_BLUE);        

        for(auto& r : rects){
            DrawRect(r.pos, r.size);
        }        

        Draw(p, olc::RED);

        DrawLine(p, mouse, olc::RED);

        //std::cout << v.x << ", " << v.y << std::endl;

        DrawString(20, 440, "Press P to set a new ray origin");

        olc::vf2d intersection;
        // if(rayToSegmentIntersection(p, v, {270.0f, 170.0f}, {20.0f, 0.0f}, intersection)){
        //     //std::cout << "aeee" << " " << intersection << std::endl;
        //     FillCircle(intersection, 5, olc::GREEN);
        // }
        int closest_rect_idx;
        float closest_dist;
        if(raycastCollision(rects, p, v, closest_rect_idx, intersection, closest_dist)){
            FillCircle(intersection, 3, olc::GREEN);
        }
        
		return true;
	}

    olc::vf2d p;
    olc::vf2d v;
    std::vector<rect> rects;

};


int main()
{
	RayCastingCollisions demo;
	if (demo.Construct(640, 480, 2, 2))
		demo.Start();

	return 0;
}