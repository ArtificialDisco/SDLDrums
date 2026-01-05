#ifndef BUTTON_H
#define BUTTON_H

const int MAXLINE = 1000;

class Button
{
 public:
   Button(SDL_Surface *screen, SDL_Surface *active, SDL_Surface *inactive,
          SDL_Surface *toggled, SDL_Rect rect, SDL_Keycode keyshortcut1,
          SDL_Keycode keyshortcut2);
   ~Button();

   void SetPosition(int x, int y);
   SDL_Keycode GetKeyShortcut() { return keyshortcut1_; }
   void PlaySample();
   bool HandleEventBase(SDL_Event *e, bool *mousedown, bool *clicked);
   // void Draw(SDL_Surface *screen);

   bool HasError() { return has_error_; }
   char *GetError() { return error_message_; }

   bool Active() { return is_active_; }
   bool Toggled() { return is_toggled_; }

   void SetActive();
   void SetInactive();
   void SetToggled(bool toggled);

 private:
   bool has_error_ = false;
   bool is_active_ = false;
   bool is_toggled_ = false;
   char error_message_[MAXLINE];

   SDL_Surface *screen_;
   SDL_Keycode keyshortcut1_;
   SDL_Keycode keyshortcut2_;
   SDL_Surface *inactive_ = nullptr;
   SDL_Surface *active_ = nullptr;
   SDL_Surface *toggled_ = nullptr;
   SDL_Rect rect_;
};

#endif  // BUTTON_H
