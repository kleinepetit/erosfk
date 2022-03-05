#include<SFML/Audio.hpp>
#include<SFML/Graphics.hpp>//使用SFML库
#include<time.h>
using namespace sf;

bool check();
void turnleft();
void turnright();
void create_blocks();

bool is_playing = false;//判断游戏是否进行
bool newrecord = false;//判断是否产生新纪录
bool hasbegun = false;//判断游戏是否已经开始
bool menu = false;//菜单是否打开
bool overcount = false;//是否打开结算界面

int last_score = 0;//最后一次分数
int high_score = 0;//最高分
int score = 0;//分数

const int row = 20;
const int col = 10;

const float qspeed = 0.05;
const float speed = 0.5;

const int ori_row = 28;
const int ori_col = 31;

int block_next = 1 + rand() % 7;;//下一个方块
int blockindex;//当前方块类型

int blocks[7][4] = {
	{1,3,5,7}//I
	,{2,3,4,5}//O
	,{2,4,5,7}//S
	,{3,5,4,6}//Z
	,{2,3,5,7}//L
	,{3,5,7,6}//J
	,{3,5,4,7}//T
};

void is_high(int* a) {
	if (*a > high_score)
	{
		high_score = *a;
		newrecord = true;//纪录新纪录及其产生
	}
}

int table[row][col] = {0};//区域和位置

void remake() {//重新开始游戏
	last_score = score;
	is_high(&score);//如果产生了最高纪录则纪录下
	score = 0;
	memset(table, 0, sizeof(table));
	create_blocks();
}

struct position
{
	int x, y;
};//用于储存方块位置

struct position current_block[4],next_block[4];

float delay = speed ;//下降速度

void moveleft() {//左移
	for (int i = 0; i < 4; i++)
	{
		current_block[i].x -= 1;
	}
	if (check() == false) {
		for (int i = 0; i < 4; i++)
		{
			current_block[i].x += 1;
		}
	}
}

void moveright() {//右移
	for (int i = 0; i < 4; i++)
	{
		current_block[i].x += 1;
	}
	if (check() == false) {
		for (int i = 0; i < 4; i++)
		{
			current_block[i].x -= 1;
		}
	}
}

void turnleft() {//左旋
	int x = current_block[1].x, y = current_block[1].y;
	int dx, dy;
	for (int i = 0; i < 4; i++)
	{
		dy = current_block[i].y - y;
		dx = current_block[i].x - x;
		current_block[i].x = -dy + x;
		current_block[i].y = dx + y;
	}
	if (check() == false) {
		turnright();//不行则回转
	}
}

void turnright() {//右旋
	int x=current_block[1].x, y=current_block[1].y;
	int dx,dy;
	for (int i = 0; i < 4; i++)
	{
		dy = current_block[i].y - y;
		dx = current_block[i].x - x;
		current_block[i].x = dy + x;
		current_block[i].y = -dx + y;
	}
	if (check() == false) {
		turnleft();//不行则回转
	}
}

void event_key(RenderWindow *window1,Sound *m1) //按键函数
{
	bool roll = false;//是否旋转
	int dx=0;
	Event event;
	while (window1->pollEvent(event)){//事件监视上下左右
		if (event.type==Event::Closed)
		{
			last_score = score;
			is_high(&score);
			window1->close();//关闭
		}
		if (event.type==Event::KeyPressed)
		{
			switch (event.key.code)
			{
			case Keyboard::Up:
				turnright();
				break;
			case Keyboard::C:
				newrecord = false;
				high_score = 0;
				last_score = 0;//清除最高分和最近分，并将产生新纪录的纪录清除
				break;
			case Keyboard::Left:
				moveleft();
				break;
			case Keyboard::Right:
				moveright();
				break;
			case Keyboard::P://暂停按钮
				hasbegun = false;
				break;
			case Keyboard::R://重新开始
				remake();
				break;
			case Keyboard::Escape://打开或关闭菜单
				if (menu)
					menu = false;
				else menu = true;
				break;
			case Keyboard::Enter://开始按钮
				if (!menu) {
					newrecord = false;
					overcount = false;
					hasbegun = true;//关闭结算和初始界面
				}//如果菜单未打开才生效
				break;
			default:
				break;
			}
		}
		if (Keyboard::isKeyPressed(Keyboard::Down))
		{
			delay = qspeed;
		}
		else
		{
			delay = speed;//按下键加速/恢复
		}
	}
}

void playmusic(Sound *m1,Sound *m2,Sound *m3) {
	if (!is_playing)
	{
		m1->play();
		m2->stop();
		m3->stop();
		is_playing = true;
	}
}//播放背景音乐

void stopmusic(Sound *m1,Sound *m2,Sound *m3) {
	m1->stop();
	is_playing = false;
	if (newrecord)
	{
		m2->play();
	}
	else
	{
		m3->play();
	}
}//播放结束音乐

void create_blocks() {
	blockindex = block_next;
	block_next = 1 + rand() % 7;
	int n = blockindex - 1;
	int m = block_next - 1;

	for (int i = 0; i < 4; i++)
	{
		current_block[i].x = blocks[n][i]%2;
		current_block[i].y = blocks[n][i] / 2;
		next_block[i].x = blocks[m][i]%2;
		next_block[i].y = blocks[m][i] / 2;
	}
}//随机生成方块

bool gameover() {//方块堆满时结束游戏
	for (int i = 0; i < col; i++)
	{
		if (table[1][i]!=0)
		{
			return true;
		}
	}
	return false;
}

void draw_blocks(Sprite *blocksimg, RenderWindow *window1) {//绘制方块
	for (int i = 0; i < row; i++)
	{
		for (int j = 0; j < col; j++)
		{
			if (table[i][j]!=0) 
			{ 
				blocksimg->setTextureRect(IntRect((table[i][j])*18,0,18,18));
				blocksimg->setPosition(j * 18, i * 18);
				blocksimg->move(ori_row, ori_col);
				//设置初始位置
				window1->draw(*blocksimg);
			}

		}
	}//底部

	for (int i = 0; i < 4; i++)
	{
		blocksimg->setTextureRect(IntRect(blockindex * 18, 0, 18, 18));
		blocksimg->setPosition(current_block[i].x*18,current_block[i].y*18);
		blocksimg->move(ori_row, ori_col);
		window1->draw(*blocksimg);
	}//运动中

	for (int i = 0; i < 4; i++)
	{
		blocksimg->setTextureRect(IntRect(block_next * 18, 0, 18, 18));
		blocksimg->setPosition(next_block[i].x * 18, next_block[i].y * 18);
		blocksimg->move(246, 60);
		window1->draw(*blocksimg);
	}//下一个
}

bool check() {
	for (int i = 0; i < 4; i++)
	{
		if (current_block[i].x < 0 || current_block[i].x>=col
			||current_block[i].y>=row
			||table[current_block[i].y][current_block[i].x]>0) {
			return false;
		}
	}
	return true;
}//检查是否触底

void drop() {//方块下落过程
	for (int i = 0; i < 4; i++)
	{
		current_block[i].y += 1;
	}
	if (check()==false)
	{
		for (int i = 0; i < 4; i++)
		{
			table[--current_block[i].y][current_block[i].x]=blockindex;//方块停止
		}
		create_blocks();
	}
}

void clearlines(Sound *m4) {//清除完整行
	int count;
	int k = row - 1;
	for (int i = row-1; i >0; i--)
	{
		count = 0;
		for (int j = 0; j < col; j++)
		{
			if (table[i][j]!=0)
			{
				count++;
			}
			table[k][j] = table[i][j];
		}
		if (count!=col)
		{
			k--;
		}
		else
		{
			score++;//加分
			m4->play();
		}
	}
}

int main(void) {

	FILE* fp;
	fopen_s(&fp, "data.txt", "r+");
	fscanf_s(fp, "%d %d", &high_score, &last_score);//读取最高和最近一次分数
	rewind(fp);

	std::string finalscore;
	srand(time(0));//生成随机种子

	RenderWindow window1(
		VideoMode(320, 416),
		"erosfk" 
	);//创建窗口

	SoundBuffer buffer0;
	buffer0.loadFromFile("music/bgmusic.ogg");
	Sound m1;
	m1.setBuffer(buffer0);
	m1.setLoop(true);
	m1.setVolume(50.f);//游戏中背景音乐

	SoundBuffer buffer1;
	buffer1.loadFromFile("music/gameover_newrecord.ogg");
	Sound m2;
	m2.setBuffer(buffer1);
	m2.setVolume(50.f);//新纪录结束音乐

	SoundBuffer buffer2;
	buffer2.loadFromFile("music/gameover_normal.ogg");
	Sound m3;
	m3.setBuffer(buffer2);
	m3.setVolume(50.f);//普通结束音乐

	SoundBuffer buffer3;
	buffer3.loadFromFile("music/clear.ogg");
	Sound m4;
	m4.setBuffer(buffer3);
	m4.setVolume(15.f);//清除行音乐

	std::string beginpage,smallwords,musicname;

	Font f1;
	f1.loadFromFile("font/Stallions - personnal use ONLY.ttf");
	Text text1;
	text1.setFont(f1);
	text1.setPosition(260, 181);
	text1.setFillColor(Color::Black);
	text1.setCharacterSize(20);//分数文字

	Font f2;
	f2.loadFromFile("font/meiryo_boot.ttf");
	Text text2;
	text2.setFont(f2);
	text2.setPosition(245, 30);
	text2.setFillColor(Color::Black);
	text2.setCharacterSize(20);
	text2.setString("Next:");//下一块文字

	Font f3;
	f3.loadFromFile("font/Stallions - personnal use ONLY.ttf");

	Texture t1;
	t1.loadFromFile("image/background_pic.png");
	Sprite backgroud(t1);//背景图片

	Texture t2;
	t2.loadFromFile("image/blocks_pic.png");
	Sprite blocksimg(t2);//方块图

	Texture t3;
	t3.loadFromFile("image/frame_pic3.png");
	Sprite frameimg(t3);//框图

	beginpage = "Press 'Esc' to open the menu\n\nPress 'Enter' to continue...\n\n High Score: " + std::to_string(high_score) + "\n\n Last Time: " + std::to_string(last_score);

	Text text3;
	text3.setString(beginpage);
	text3.setCharacterSize(20);
	text3.setFillColor(Color::Black);
	text3.setFont(f2);
	FloatRect ff1=text3.getGlobalBounds();
	text3.setPosition(30, 100);//开始界面文字

	Text text4;
	text4.setString("Esc ----- Open/Close the menu\nEnter ----- Begin\nP ----- Pause\nR ----- Remake\nC ----- Clear records\nUp ----- Turn the block\nDown ----- Speed up\nLeft ----- Move to left\nRight ----- Move to right");
	text4.setCharacterSize(20);
	text4.setFillColor(Color::Black);
	text4.setFont(f2);
	FloatRect ff2 = text4.getGlobalBounds();
	text4.setOrigin(ff2.width / 2, ff2.height / 2);
	text4.setPosition(160, 208);//菜单界面文字

	Text text5;
	text5.setString(smallwords);
	text5.setCharacterSize(15);
	text5.setFillColor(Color::Black);
	text5.setFont(f3);
	text5.setPosition(230,245);//当前最高分和最近一次分数显示

	create_blocks();

	Clock clock1;
	float ttime = 0;

	while (window1.isOpen())// 窗口循环
	{	
		is_high(&score);
		event_key(&window1,&m1);
		if (!overcount && !hasbegun && !menu) {//如果未开始则打开开始界面
			window1.draw(backgroud);
			window1.draw(text3);
			window1.display();
		}
		else if (menu)//如果按了菜单键就打开菜单
		{
			window1.draw(backgroud);
			window1.draw(text4);
			window1.display();
		}
		else if (overcount) {//如果进入结算界面
			if (newrecord == true) {
				beginpage = "A new record has been set!\n\nYour score is " + std::to_string(high_score) + "\n\nPress 'Enter' to continue";
			}
			else beginpage = "Your score is " + std::to_string(last_score) + "\n\nPress 'Enter' to continue";
			text3.setString(beginpage);
			window1.draw(backgroud);
			window1.draw(text3);
			window1.display();
		}
		else
		{
			playmusic(&m1,&m2,&m3);//播放游戏背景音乐
			if (gameover()) {
				stopmusic(&m1, &m2, &m3);//停止bgm并播放结束音
				last_score = score;
				is_high(&score);
				score = 0;
				memset(table, 0, sizeof(table));
				hasbegun = false;
				overcount = true;
			}//游戏结束清空界面

			float time = clock1.getElapsedTime().asSeconds();//获取时间
			clock1.restart();
			ttime += time;

			clearlines(&m4);

			if (ttime>delay)
			{
				drop();
				ttime = 0;
			}

			window1.draw(backgroud);//显示背景
			window1.draw(frameimg);//显示框图

			finalscore = std::to_string(score);
			text1.setString(finalscore);
			smallwords = "High Score :\n" + std::to_string(high_score) + "\nLast Time :\n" + std::to_string(last_score);
			text5.setString(smallwords);
			window1.draw(text1);
			window1.draw(text2);//显示文本
			window1.draw(text5);

			draw_blocks(&blocksimg,&window1);

			window1.display();
		}
		
	}
	fprintf_s(fp, "%d %d\n", high_score, last_score);
	fclose(fp);//将最大分数和最近分数输入文件并关闭文件
	return 0;
}