#include <QVBoxLayout>

#include <iostream>

#include "dialog.h"
#include "iplayer.h"
#include "message.h"
#include "curl2.h"

using namespace std;

MusicDialog::MusicDialog()
{
	pthread_create(&thr_iplayer, NULL, play_func, NULL);
	
	download("http://127.0.0.1:8080/examples/1.jpg");

	createButtons();
	createActions();
	
}

void MusicDialog::createButtons()
{
	startButton = new QPushButton("Start");
	stopButton = new QPushButton("Stop");
	continueButton = new QPushButton("Continue");
	nextButton = new QPushButton("Next");
	exitButton = new QPushButton("Exit");

	label = new QLabel;
	QPixmap *pixmap = new QPixmap("./1.jpg"); 
	label->setPixmap(*pixmap);
	label->show();

	QHBoxLayout *layout = new QHBoxLayout;
	
	layout->addWidget(label);
	layout->addWidget(startButton);
	layout->addWidget(stopButton);
	layout->addWidget(continueButton);
	layout->addWidget(nextButton);
	layout->addWidget(exitButton);

	setLayout(layout);

	stopButton->setDisabled(true);
	continueButton->setDisabled(true);
	nextButton->setDisabled(true);
}

void MusicDialog::createActions()
{
	connect(startButton, SIGNAL(clicked()), this, SLOT(startMusic()));
	connect(stopButton, SIGNAL(clicked()), this, SLOT(stopMusic()));
	connect(continueButton, SIGNAL(clicked()), this, SLOT(continueMusic()));
	connect(nextButton, SIGNAL(clicked()), this, SLOT(nextMusic()));
	connect(exitButton, SIGNAL(clicked()), this, SLOT(exitMusic()));	
}

void MusicDialog::startMusic()
{

	cout<<"start music"<<endl;
	iputchar('s');

	startButton->setDisabled(true);	
	stopButton->setDisabled(false);
	nextButton->setDisabled(false);

}

void MusicDialog::stopMusic()
{

	iputchar('c');
	continueButton->setDisabled(false);
	stopButton->setDisabled(true);
	nextButton->setDisabled(true);

}

void MusicDialog::continueMusic()
{

	iputchar('g');
	continueButton->setDisabled(true);
	stopButton->setDisabled(false);
	nextButton->setDisabled(false);
}

void MusicDialog::nextMusic()
{

	iputchar('n');
	continueButton->setDisabled(true);
	stopButton->setDisabled(false);

}

void MusicDialog::exitMusic()
{
	exit(0);
}

int main(int argc, char **argv)
{
	QApplication app(argc, argv);
	MusicDialog *dialog = new MusicDialog;
	dialog->show();
	return app.exec();
}
