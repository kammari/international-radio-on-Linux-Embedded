#ifndef MUSIC
#define MUSIC

#include <QtGui>
#include <QVBoxLayout>

class MusicDialog : public QDialog
{
		Q_OBJECT
	public :
		MusicDialog();
		void createButtons();
		void createPicture();
		void createActions();

	private slots :
		void startMusic();
		void stopMusic();
		void continueMusic();
		void nextMusic();
		void exitMusic();

	private :
		QPushButton *startButton;
		QPushButton *stopButton;
		QPushButton *continueButton;
		QPushButton *nextButton;
		QPushButton *exitButton;
		QLabel 		*label;
		pthread_t thr_iplayer;
};

#endif
