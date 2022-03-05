#include<SFML/Audio.hpp>
#include<SFML/Graphics.hpp>//ʹ��SFML��
#include<time.h>
using namespace sf;

bool check();
void turnleft();
void turnright();
void create_blocks();

bool is_playing = false;//�ж���Ϸ�Ƿ����
bool newrecord = false;//�ж��Ƿ�����¼�¼
bool hasbegun = false;//�ж���Ϸ�Ƿ��Ѿ���ʼ
bool menu = false;//�˵��Ƿ��
bool overcount = false;//�Ƿ�򿪽������

int last_score = 0;//���һ�η���
int high_score = 0;//��߷�
int score = 0;//����

const int row = 20;
const int col = 10;

const float qspeed = 0.05;
const float speed = 0.5;

const int ori_row = 28;
const int ori_col = 31;

int block_next = 1 + rand() % 7;;//��һ������
int blockindex;//��ǰ��������

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
		newrecord = true;//��¼�¼�¼�������
	}
}

int table[row][col] = {0};//�����λ��

void remake() {//���¿�ʼ��Ϸ
	last_score = score;
	is_high(&score);//�����������߼�¼���¼��
	score = 0;
	memset(table, 0, sizeof(table));
	create_blocks();
}

struct position
{
	int x, y;
};//���ڴ��淽��λ��

struct position current_block[4],next_block[4];

float delay = speed ;//�½��ٶ�

void moveleft() {//����
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

void moveright() {//����
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

void turnleft() {//����
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
		turnright();//�������ת
	}
}

void turnright() {//����
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
		turnleft();//�������ת
	}
}

void event_key(RenderWindow *window1,Sound *m1) //��������
{
	bool roll = false;//�Ƿ���ת
	int dx=0;
	Event event;
	while (window1->pollEvent(event)){//�¼�������������
		if (event.type==Event::Closed)
		{
			last_score = score;
			is_high(&score);
			window1->close();//�ر�
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
				last_score = 0;//�����߷ֺ�����֣����������¼�¼�ļ�¼���
				break;
			case Keyboard::Left:
				moveleft();
				break;
			case Keyboard::Right:
				moveright();
				break;
			case Keyboard::P://��ͣ��ť
				hasbegun = false;
				break;
			case Keyboard::R://���¿�ʼ
				remake();
				break;
			case Keyboard::Escape://�򿪻�رղ˵�
				if (menu)
					menu = false;
				else menu = true;
				break;
			case Keyboard::Enter://��ʼ��ť
				if (!menu) {
					newrecord = false;
					overcount = false;
					hasbegun = true;//�رս���ͳ�ʼ����
				}//����˵�δ�򿪲���Ч
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
			delay = speed;//���¼�����/�ָ�
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
}//���ű�������

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
}//���Ž�������

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
}//������ɷ���

bool gameover() {//�������ʱ������Ϸ
	for (int i = 0; i < col; i++)
	{
		if (table[1][i]!=0)
		{
			return true;
		}
	}
	return false;
}

void draw_blocks(Sprite *blocksimg, RenderWindow *window1) {//���Ʒ���
	for (int i = 0; i < row; i++)
	{
		for (int j = 0; j < col; j++)
		{
			if (table[i][j]!=0) 
			{ 
				blocksimg->setTextureRect(IntRect((table[i][j])*18,0,18,18));
				blocksimg->setPosition(j * 18, i * 18);
				blocksimg->move(ori_row, ori_col);
				//���ó�ʼλ��
				window1->draw(*blocksimg);
			}

		}
	}//�ײ�

	for (int i = 0; i < 4; i++)
	{
		blocksimg->setTextureRect(IntRect(blockindex * 18, 0, 18, 18));
		blocksimg->setPosition(current_block[i].x*18,current_block[i].y*18);
		blocksimg->move(ori_row, ori_col);
		window1->draw(*blocksimg);
	}//�˶���

	for (int i = 0; i < 4; i++)
	{
		blocksimg->setTextureRect(IntRect(block_next * 18, 0, 18, 18));
		blocksimg->setPosition(next_block[i].x * 18, next_block[i].y * 18);
		blocksimg->move(246, 60);
		window1->draw(*blocksimg);
	}//��һ��
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
}//����Ƿ񴥵�

void drop() {//�����������
	for (int i = 0; i < 4; i++)
	{
		current_block[i].y += 1;
	}
	if (check()==false)
	{
		for (int i = 0; i < 4; i++)
		{
			table[--current_block[i].y][current_block[i].x]=blockindex;//����ֹͣ
		}
		create_blocks();
	}
}

void clearlines(Sound *m4) {//���������
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
			score++;//�ӷ�
			m4->play();
		}
	}
}

int main(void) {

	FILE* fp;
	fopen_s(&fp, "data.txt", "r+");
	fscanf_s(fp, "%d %d", &high_score, &last_score);//��ȡ��ߺ����һ�η���
	rewind(fp);

	std::string finalscore;
	srand(time(0));//�����������

	RenderWindow window1(
		VideoMode(320, 416),
		"erosfk" 
	);//��������

	SoundBuffer buffer0;
	buffer0.loadFromFile("music/bgmusic.ogg");
	Sound m1;
	m1.setBuffer(buffer0);
	m1.setLoop(true);
	m1.setVolume(50.f);//��Ϸ�б�������

	SoundBuffer buffer1;
	buffer1.loadFromFile("music/gameover_newrecord.ogg");
	Sound m2;
	m2.setBuffer(buffer1);
	m2.setVolume(50.f);//�¼�¼��������

	SoundBuffer buffer2;
	buffer2.loadFromFile("music/gameover_normal.ogg");
	Sound m3;
	m3.setBuffer(buffer2);
	m3.setVolume(50.f);//��ͨ��������

	SoundBuffer buffer3;
	buffer3.loadFromFile("music/clear.ogg");
	Sound m4;
	m4.setBuffer(buffer3);
	m4.setVolume(15.f);//���������

	std::string beginpage,smallwords,musicname;

	Font f1;
	f1.loadFromFile("font/Stallions - personnal use ONLY.ttf");
	Text text1;
	text1.setFont(f1);
	text1.setPosition(260, 181);
	text1.setFillColor(Color::Black);
	text1.setCharacterSize(20);//��������

	Font f2;
	f2.loadFromFile("font/meiryo_boot.ttf");
	Text text2;
	text2.setFont(f2);
	text2.setPosition(245, 30);
	text2.setFillColor(Color::Black);
	text2.setCharacterSize(20);
	text2.setString("Next:");//��һ������

	Font f3;
	f3.loadFromFile("font/Stallions - personnal use ONLY.ttf");

	Texture t1;
	t1.loadFromFile("image/background_pic.png");
	Sprite backgroud(t1);//����ͼƬ

	Texture t2;
	t2.loadFromFile("image/blocks_pic.png");
	Sprite blocksimg(t2);//����ͼ

	Texture t3;
	t3.loadFromFile("image/frame_pic3.png");
	Sprite frameimg(t3);//��ͼ

	beginpage = "Press 'Esc' to open the menu\n\nPress 'Enter' to continue...\n\n High Score: " + std::to_string(high_score) + "\n\n Last Time: " + std::to_string(last_score);

	Text text3;
	text3.setString(beginpage);
	text3.setCharacterSize(20);
	text3.setFillColor(Color::Black);
	text3.setFont(f2);
	FloatRect ff1=text3.getGlobalBounds();
	text3.setPosition(30, 100);//��ʼ��������

	Text text4;
	text4.setString("Esc ----- Open/Close the menu\nEnter ----- Begin\nP ----- Pause\nR ----- Remake\nC ----- Clear records\nUp ----- Turn the block\nDown ----- Speed up\nLeft ----- Move to left\nRight ----- Move to right");
	text4.setCharacterSize(20);
	text4.setFillColor(Color::Black);
	text4.setFont(f2);
	FloatRect ff2 = text4.getGlobalBounds();
	text4.setOrigin(ff2.width / 2, ff2.height / 2);
	text4.setPosition(160, 208);//�˵���������

	Text text5;
	text5.setString(smallwords);
	text5.setCharacterSize(15);
	text5.setFillColor(Color::Black);
	text5.setFont(f3);
	text5.setPosition(230,245);//��ǰ��߷ֺ����һ�η�����ʾ

	create_blocks();

	Clock clock1;
	float ttime = 0;

	while (window1.isOpen())// ����ѭ��
	{	
		is_high(&score);
		event_key(&window1,&m1);
		if (!overcount && !hasbegun && !menu) {//���δ��ʼ��򿪿�ʼ����
			window1.draw(backgroud);
			window1.draw(text3);
			window1.display();
		}
		else if (menu)//������˲˵����ʹ򿪲˵�
		{
			window1.draw(backgroud);
			window1.draw(text4);
			window1.display();
		}
		else if (overcount) {//�������������
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
			playmusic(&m1,&m2,&m3);//������Ϸ��������
			if (gameover()) {
				stopmusic(&m1, &m2, &m3);//ֹͣbgm�����Ž�����
				last_score = score;
				is_high(&score);
				score = 0;
				memset(table, 0, sizeof(table));
				hasbegun = false;
				overcount = true;
			}//��Ϸ������ս���

			float time = clock1.getElapsedTime().asSeconds();//��ȡʱ��
			clock1.restart();
			ttime += time;

			clearlines(&m4);

			if (ttime>delay)
			{
				drop();
				ttime = 0;
			}

			window1.draw(backgroud);//��ʾ����
			window1.draw(frameimg);//��ʾ��ͼ

			finalscore = std::to_string(score);
			text1.setString(finalscore);
			smallwords = "High Score :\n" + std::to_string(high_score) + "\nLast Time :\n" + std::to_string(last_score);
			text5.setString(smallwords);
			window1.draw(text1);
			window1.draw(text2);//��ʾ�ı�
			window1.draw(text5);

			draw_blocks(&blocksimg,&window1);

			window1.display();
		}
		
	}
	fprintf_s(fp, "%d %d\n", high_score, last_score);
	fclose(fp);//����������������������ļ����ر��ļ�
	return 0;
}