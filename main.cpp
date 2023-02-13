#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

struct rect{
    olc::vf2d pos;
    olc::vf2d size;
};

bool rayToSegmentIntersection(olc::vf2d p, olc::vf2d v, olc::vf2d q, olc::vf2d w, olc::vf2d& intersection){
    // l(t) = p + t * v (ray segment)
    // r(s) = q + s * w (considering the line segment where 0 <= s <= 1)

    const float epsilon = 0.000001f;
    float det = (v.y * w.x - v.x * w.y);
    if (fabs(det) < epsilon){
        return false;
    }

    float dx = q.x - p.x;
    float dy = q.y - p.y;

    float t = (dy * w.x - dx * w.y) / det;
    if (t < 0 || t > 1){
        return false;
    }

    float s = (dy * v.x - dx * v.y) / det;
    if (s < 0 || s > 1){
        return false;
    }

    intersection = p + t * v;

    return true;
}

bool raycastCollision(const std::vector<rect>& rects, olc::vf2d p, olc::vf2d v){

    for(auto& rect : rects){



    }

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

        olc::vf2d intersection;
        if(rayToSegmentIntersection(p, v, {270.0f, 170.0f}, {20.0f, 0.0f}, intersection)){
            //std::cout << "aeee" << " " << intersection << std::endl;
            FillCircle(intersection, 5, olc::GREEN);
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