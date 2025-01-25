#include <iostream>
#include <raylib.h>
#include <deque>
#include <raymath.h>

using namespace std;

Color blue = { 159, 193, 224, 255 };
Color darkBlue = { 57, 54, 117, 255 };

int cellSize = 30;
int cellCount = 15;
int offset = 75;



double lastUpdateTime = 0;

bool SnakeCollision(Vector2 element, deque<Vector2> deque)
{
	for (unsigned int i = 1; i < deque.size();i++)
	{
		if (Vector2Equals(deque[i], element))
		{
			return true;
		}
	}
	return false;
}

bool ElementInDeque(Vector2 element, deque<Vector2> deque)
{
	for (unsigned int i = 0; i < deque.size();i++)
	{
		if (Vector2Equals(deque[i], element))
		{
			return true;
		}
	}
	return false;
}

bool eventTrigger(double interval)
{
	double currentTime = GetTime();
	if (currentTime - lastUpdateTime >= interval)
	{
		lastUpdateTime = currentTime;
		return true;
	}
	return false;
}

class Snake {
public:
	Vector2 head = { 6,9 };
	Vector2 mid = { 5,9 };
	Vector2 tail = { 4,9 };
	deque<Vector2> body = { head,mid,tail };
	Vector2 direction = { 1,0 }; 
	bool addSegment = false;

	void Draw()
	{
		for (unsigned int i = 0; i < body.size();i++)
		{
			float x = body[i].x;
			float y = body[i].y;
			Rectangle segment = Rectangle{ offset + x * cellSize,offset + y * cellSize, (float)cellSize, (float)cellSize };
			DrawRectangleRounded(segment, 0.5, 6, darkBlue);
		}
	}

	void Update()
	{
		body.push_front(Vector2Add(body[0], direction));
		if (addSegment == true)
		{
			addSegment = false;
		}
		else
		{
			body.pop_back();
		}
		
	}

	void Reset()
	{
		body = { Vector2{6, 9}, Vector2{5, 9}, Vector2{4, 9} };
		direction = { 1, 0 };
	}
};


class Food {
public:
	Vector2 position;
	Texture2D texture;

	Food(deque<Vector2> snakeBody)
	{
		Image image = LoadImage("Graphics/GrnApple.png");
		texture = LoadTextureFromImage(image);
		UnloadImage(image);
		position = GenerateRandomPos(snakeBody);

	}
	
	~Food()
	{
		UnloadTexture(texture);
	}

	void Draw()
	{
		DrawTexture(texture, offset + position.x * cellSize, offset + position.y * cellSize, WHITE);
	}

	Vector2 GenerateRandomCell()
	{
		float x = GetRandomValue(0, cellCount - 1);
		float y = GetRandomValue(0, cellCount - 1);
		return Vector2{ x,y };
	}

	Vector2 GenerateRandomPos(deque<Vector2>snakeBody)
	{
		Vector2 position = GenerateRandomCell();
		while (ElementInDeque(position, snakeBody))
		{
			position = GenerateRandomCell();
		}
		return position;
	}

};


class Game 
{
public:
	Snake snake = Snake();
	Food food = Food(snake.body);
	bool running = true;
	int score = 0;
	Sound eatSound;
	Sound crashSound;

	Game()
	{
		InitAudioDevice();
		eatSound = LoadSound("Sounds/eat.mp3");
		crashSound = LoadSound("Sounds/crash.mp3");
	}

	~Game()
	{
		UnloadSound(eatSound);
		UnloadSound(crashSound);
		CloseAudioDevice();
	}
	
	void Draw()
	{
		if (running)
		{
			food.Draw();
			snake.Draw();
		}
		
	}

	void Update()
	{
		if (running)
		{
			snake.Update();
			CheckCollisionWithFood();
			CheckCollisionWithEdges();
			CheckCollisionWithBody();
		}
		
	}

	void gameOver()
	{
		cout << "You Died!" << endl;
		cout << "Your Score: " << score << endl;
		snake.Reset();
		food.position = food.GenerateRandomPos(snake.body);
		running = false;
		
	}

	
	void CheckCollisionWithBody()
	{
		if (SnakeCollision(snake.body[0], snake.body) == true)
		{
			PlaySound(crashSound);
			gameOver();
			
		}
	}

	void CheckCollisionWithFood()
	{
		if (Vector2Equals(snake.body[0], food.position))
		{
			food.position = food.GenerateRandomPos(snake.body);
			snake.addSegment = true;
			score++;
			PlaySound(eatSound);
		}
	}

	void CheckCollisionWithEdges()
	{
		if (snake.body[0].x >= cellCount) {
			snake.body[0].x = 0; 
		}
		else if (snake.body[0].x < 0) {
			snake.body[0].x = cellCount - 1; 
		}

		if (snake.body[0].y >= cellCount) {
			snake.body[0].y = 0; 
		}
		else if (snake.body[0].y < 0) {
			snake.body[0].y = cellCount - 1;
		}
	}

	

};


int main()
{

	cout << "Starting the game..." << endl;
	InitWindow(2 * offset + cellSize * cellCount, 2 * offset + cellSize * cellCount, "Snake Game");
	SetTargetFPS(60);

	Game game = Game();

	while(WindowShouldClose() == false)
	{
		BeginDrawing();

		if (eventTrigger(0.25))
		{
			game.Update();
		}

		if (IsKeyPressed(KEY_W) && game.snake.direction.y != 1 || IsKeyPressed(KEY_UP) && game.snake.direction.y != 1)
		{
			game.snake.direction = { 0, -1 };
			
		}
		else if (IsKeyPressed(KEY_S) && game.snake.direction.y != -1 || IsKeyPressed(KEY_DOWN) && game.snake.direction.y != -1)
		{
			game.snake.direction = { 0, 1 };
			
		}
		else if (IsKeyPressed(KEY_A) && game.snake.direction.x != 1 || IsKeyPressed(KEY_LEFT) && game.snake.direction.x != 1)
		{
			game.snake.direction = { -1, 0 };
			
		}
		else if (IsKeyPressed(KEY_D) && game.snake.direction.x != -1 || IsKeyPressed(KEY_RIGHT) && game.snake.direction.x != -1)
		{
			game.snake.direction = { 1, 0 };
		}
		if (IsKeyPressed(KEY_ENTER) && game.running == false)
		{

			game.running = true;
			game.score = 0;
		}


		ClearBackground(blue);
		DrawRectangleLinesEx(Rectangle{ (float)offset - 5,(float)offset - 5,(float)cellSize * cellCount + 10,(float)cellSize * cellCount + 10 }, 5, darkBlue);
		DrawText("Snake Game 2.0", offset-5, 20, 40, darkBlue);
		DrawText(TextFormat("Score: %i",game.score), offset - 5, offset + cellSize * cellCount + 10, 30, darkBlue);
		game.Draw();

		if (game.running == false)
		{
			DrawText(TextFormat("Score: %i", game.score), offset + cellCount * cellSize / 2 - MeasureText(TextFormat("Score: %i", game.score), 40) / 2, cellCount * cellSize / 2 - 60 - 40, 40, darkBlue);
			DrawText("GAME OVER!", offset + cellCount * cellSize / 2 - MeasureText("GAME OVER!", 60) / 2, cellCount * cellSize / 2 - 60 / 2, 60, darkBlue);
			DrawText("	Click ENTER to Restart!\n			Click Esc to Exit!", offset + cellCount * cellSize / 2 - MeasureText("	Click ENTER to Restart!\nClick Esc to Exit!", 30) / 2, cellCount * cellSize / 2 + 60 + 40, 30, darkBlue);
		}

		EndDrawing();
	}

	CloseWindow();
	return 0;
}