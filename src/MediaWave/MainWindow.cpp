#include "pch.h"
#include "pixmaps.h"
#include "PlotWidget.h"
#include "MediumModel.h"
#include "MainWindow.h"

MainWindow::MainWindow(MediumModel* model)
    : Fl_Window(700, 500, "MediaWave"), model_(model) {
    create_settings_wnd();

    this->resizable(this);
    this->begin();

    uw = new PlotWidget(10, 10, 480, 235, "u(x)");
    pw = new PlotWidget(10, 255, 480, 235, "p(x)");

    settings_btn = new Fl_Button(550, 20, 90, 25, "Settings");
    step_btn = new Fl_Button(550, 50, 90, 25, "Step");
    start_btn = new Fl_Button(550, 80, 90, 25, "Start");
    stop_btn = new Fl_Button(550, 110, 90, 25, "Stop");

    this->end();

    step_btn->callback(step_cb_st, this);
    start_btn->callback(start_cb_st, this);
    stop_btn->callback(stop_cb_st, this);
    settings_btn->callback(settings_cb_st, this);

    stop_btn->deactivate();

    uw->view_range(0.0, DefL, -2.0, 2.0);
    uw->ticks(DefTickCount, DefTickSize);

    pw->view_range(0.0, DefL, -2.0, 2.0);
    pw->ticks(DefTickCount, DefTickSize);

    uw->plot(DefN, model->x, model->u1);
    pw->plot(DefN, model->x, model->p1);

    helper_.L = DefL;
    helper_.N = DefN;
    helper_.a = DefA;
    helper_.rho = DefRho;
    helper_.sigma = DefSigma;
    helper_.un_type = DefUNType;
    helper_.pn_type = DefPNType;
    helper_.left_side_type = SIDE_CLOSE;
    helper_.right_side_type = SIDE_CLOSE;
    helper_.bl = 1.0;
    helper_.cl = 0.0;
    helper_.br = 1.0;
    helper_.cr = 0.0;

    helper(helper_);
}

void MainWindow::redraw() {
    uw->plot(model_->N, model_->x, model_->u1);
    pw->plot(model_->N, model_->x, model_->p1);

    uw->redraw();
    pw->redraw();
}

void MainWindow::helper(SettingsHelper h) {
    void* side;

    helper_ = h;

    model_->L = helper_.L;
    model_->N = helper_.N;

    model_->a = helper_.a;
    model_->rho = helper_.rho;
    model_->sigma = helper_.sigma;

    model_->InitFunc(helper_.un_type, helper_.pn_type);

    side = helper_.left_side_type;
    if (side == SIDE_CLOSE) model_->InitCondL(INIT_COND_CLOSED);
    else if (side == SIDE_OPEN)  model_->InitCondL(INIT_COND_OPENED);
    else if (side == SIDE_MANUAL) model_->InitCondL(helper_.bl, helper_.cl);

    side = helper_.right_side_type;
    if (side == SIDE_CLOSE) model_->InitCondR(INIT_COND_CLOSED);
    else if (side == SIDE_OPEN)  model_->InitCondR(INIT_COND_OPENED);
    else if (side == SIDE_MANUAL) model_->InitCondR(helper_.br, helper_.cr);

    uw->view_range(0.0, model_->L, -2.0, 2.0);
    uw->ticks(DefTickCount, DefTickSize);
    uw->invalidate();

    pw->view_range(0.0, model_->L, -2.0, 2.0);
    pw->ticks(DefTickCount, DefTickSize);
    pw->invalidate();

    this->model_->Reset();
    this->redraw();
}

SettingsHelper MainWindow::helper() {
    return this->helper_;
}

void MainWindow::step_cb_st(Fl_Widget*, void* v) {
    MainWindow* w = static_cast<MainWindow*>(v);
    w->step_cb();
}

void MainWindow::step_cb() {
    this->model_->Step();
    this->redraw();
}

void MainWindow::start_cb_st(Fl_Widget*, void* v) {
    MainWindow* w = static_cast<MainWindow*>(v);
    w->start_cb();
}

void MainWindow::start_cb() {
    this->settings_btn->deactivate();
    this->step_btn->deactivate();
    this->start_btn->deactivate();
    this->stop_btn->activate();

    Fl::add_timeout(TimerInterval, timer_cb_st, this);
}

void MainWindow::stop_cb_st(Fl_Widget*, void* v) {
    MainWindow* w = static_cast<MainWindow*>(v);
    w->stop_cb();
}

void MainWindow::stop_cb() {
    this->settings_btn->activate();
    this->step_btn->activate();
    this->start_btn->activate();
    this->stop_btn->deactivate();

    Fl::remove_timeout(timer_cb_st, this);
}

void MainWindow::settings_cb_st(Fl_Widget*, void* v) {
    MainWindow* w = static_cast<MainWindow*>(v);
    w->settings_cb();
}

void MainWindow::settings_cb() {
    this->set_inputs(helper_);
    settings_wnd->show();
}

void MainWindow::timer_cb_st(void* v) {
    MainWindow* w = static_cast<MainWindow*>(v);
    w->timer_cb();
}

void MainWindow::timer_cb() {
    this->model_->Step();
    this->redraw();

    Fl::repeat_timeout(TimerInterval, timer_cb_st, this);
}

// Settings window
const char label_close_left[] = "[==";
const char label_close_right[] = "==]";
const char label_open[] = "===";
const char label_manual[] = "~~~";

void MainWindow::create_side_selector(Fl_Tree* t) {
    t->showroot(0);
    t->connectorstyle(FL_TREE_CONNECTOR_NONE);
    t->selectmode(FL_TREE_SELECT_SINGLE);
    t->connectorwidth(0);
    t->marginleft(0);
    t->visible_focus(0);
}

void MainWindow::set_tree_icons(Fl_Tree* t) {
    static Fl_Pixmap L_close_l(L_close_l_xpm);
    static Fl_Pixmap L_close_r(L_close_r_xpm);
    static Fl_Pixmap L_open(L_open_xpm);
    static Fl_Pixmap L_manual(L_manual_xpm);

    for (Fl_Tree_Item* item = t->first(); item; item = item->next()) {
        if (strcmp(item->label(), label_close_left) == 0)
            item->usericon(&L_close_l);
        else if (strcmp(item->label(), label_open) == 0)
            item->usericon(&L_open);
        else if (strcmp(item->label(), label_close_right) == 0)
            item->usericon(&L_close_r);
        else if (strcmp(item->label(), label_manual) == 0)
            item->usericon(&L_manual);

        item->label("");
    }
}

void MainWindow::select_side_type(Fl_Tree* t, void* type) {
    for (Fl_Tree_Item* item = t->first(); item; item = item->next()) {
        if (item->user_data() == type) {
            t->select_only(item);
            return;
        }
    }
}

void MainWindow::create_settings_wnd() {
    settings_wnd = new Fl_Window(320, 480, "Settings");

    settings_wnd->begin();

    len_in = new Fl_Input(70, 10, 90, 25, "L = ");
    n_in = new Fl_Input(70, 40, 90, 25, "N = ");
    a_in = new Fl_Input(70, 80, 90, 25, "a = ");
    rho_in = new Fl_Input(70, 110, 90, 25, "rho = ");
    sigma_in = new Fl_Input(70, 140, 90, 25, "sigma = ");

    u_choice = new Fl_Choice(220, 10, 90, 25, "U(x)");
    u_choice->add("- - -");
    u_choice->add("^ - ^");
    u_choice->add("^ - -");
    u_choice->add("- ^ -");
    u_choice->add("- - ^");
    u_choice->add(" V ^ ");
    u_choice->add(" ^ V ");

    p_choice = new Fl_Choice(220, 40, 90, 25, "P(x)");
    p_choice->add("- - -");
    p_choice->add("^ - ^");
    p_choice->add("^ - -");
    p_choice->add("- ^ -");
    p_choice->add("- - ^");

    left_side_choice = new Fl_Tree(90, 200, 52, 116, "Left");
    create_side_selector(left_side_choice);
    left_side_choice->clear();
    left_side_choice->add(label_close_left)->user_data(SIDE_CLOSE);
    left_side_choice->add(label_open)->user_data(SIDE_OPEN);
    left_side_choice->add(label_manual)->user_data(SIDE_MANUAL);
    set_tree_icons(left_side_choice);
    left_side_choice->callback(left_side_cb_st, this);
    bl_in = new Fl_Input(70, 330, 90, 25, "bl");
    cl_in = new Fl_Input(70, 360, 90, 25, "cl");

    right_side_choice = new Fl_Tree(210, 200, 52, 116, "Right");
    create_side_selector(right_side_choice);
    right_side_choice->clear();
    right_side_choice->add(label_close_right)->user_data(SIDE_CLOSE);
    right_side_choice->add(label_open)->user_data(SIDE_OPEN);
    right_side_choice->add(label_manual)->user_data(SIDE_MANUAL);
    set_tree_icons(right_side_choice);
    right_side_choice->callback(right_side_cb_st, this);
    br_in = new Fl_Input(190, 330, 90, 25, "br");
    cr_in = new Fl_Input(190, 360, 90, 25, "cr");

    apply_btn = new Fl_Button(155, 450, 75, 23, "Apply");
    close_btn = new Fl_Button(235, 450, 75, 23, "Close");

    apply_btn->callback(apply_cb_st, this);
    close_btn->callback(close_cb_st, this);

    settings_wnd->end();
    settings_wnd->set_modal();
}

void MainWindow::set_inputs(SettingsHelper helper) {
    static char str[30];

    u_choice->value(helper.un_type);
    p_choice->value(helper.pn_type);
    set_lcond_inputs(helper);
    set_rcond_inputs(helper);

    sprintf(str, "%.4f", helper.L);
    len_in->value(str);

    sprintf(str, "%d", helper.N);
    n_in->value(str);

    sprintf(str, "%.4f", helper.a);
    a_in->value(str);

    sprintf(str, "%.4f", helper.rho);
    rho_in->value(str);

    sprintf(str, "%.4f", helper.sigma);
    sigma_in->value(str);
}

SettingsHelper MainWindow::get_inputs() {
    static char str[30];
    SettingsHelper helper;

    helper.un_type = u_choice->value();
    helper.pn_type = p_choice->value();
    get_lcond_inputs(helper);
    get_rcond_inputs(helper);

    strcpy(str, len_in->value());
    helper.L = atof(str);

    strcpy(str, n_in->value());
    helper.N = atoi(str);

    strcpy(str, a_in->value());
    helper.a = atof(str);

    strcpy(str, rho_in->value());
    helper.rho = atof(str);

    strcpy(str, sigma_in->value());
    helper.sigma = atof(str);

    return helper;
}

void MainWindow::set_lcond_inputs(SettingsHelper helper) {
    static char str[30];
    select_side_type(left_side_choice, helper.left_side_type);

    sprintf(str, "%.1f", helper.bl);
    bl_in->value(str);

    sprintf(str, "%.1f", helper.cl);
    cl_in->value(str);
}

void MainWindow::get_lcond_inputs(SettingsHelper& helper) {
    static char str[30];
    Fl_Tree_Item* item = left_side_choice->first_selected_item();
    helper.left_side_type = item->user_data();

    if (helper.left_side_type == SIDE_MANUAL) {
        strcpy(str, bl_in->value());
        helper.bl = atof(str);

        strcpy(str, cl_in->value());
        helper.cl = atof(str);

    }
    else if (helper.left_side_type == SIDE_CLOSE) {
        helper.cl = 0.0;
        helper.bl = 1.0;

    }
    else if (helper.left_side_type == SIDE_OPEN) {
        helper.cl = 0.0;
        helper.bl = 1.0;
    }
}

void MainWindow::set_rcond_inputs(SettingsHelper helper) {
    static char str[30];
    select_side_type(right_side_choice, helper.right_side_type);

    sprintf(str, "%.1f", helper.br);
    br_in->value(str);

    sprintf(str, "%.1f", helper.cr);
    cr_in->value(str);
}

void MainWindow::get_rcond_inputs(SettingsHelper& helper) {
    static char str[30];
    Fl_Tree_Item* item = right_side_choice->first_selected_item();
    helper.right_side_type = item->user_data();

    if (helper.right_side_type == SIDE_MANUAL) {
        strcpy(str, br_in->value());
        helper.br = atof(str);

        strcpy(str, cr_in->value());
        helper.cr = atof(str);

    }
    else if (helper.right_side_type == SIDE_CLOSE) {
        helper.cr = 0.0;
        helper.br = 1.0;

    }
    else if (helper.right_side_type == SIDE_OPEN) {
        helper.cr = 0.0;
        helper.br = 1.0;
    }
}

void MainWindow::close_cb_st(Fl_Widget*, void* v) {
    MainWindow* w = (MainWindow*)v;
    w->close_cb();
}

void MainWindow::close_cb() {
    settings_wnd->hide();
}

void MainWindow::apply_cb_st(Fl_Widget*, void* v) {
    MainWindow* w = (MainWindow*)v;
    w->apply_cb();
}

void MainWindow::apply_cb() {
    settings_wnd->hide();
    this->helper(this->get_inputs());
}

void MainWindow::left_side_cb_st(Fl_Widget*, void* v) {
    MainWindow* w = (MainWindow*)v;
    w->left_side_cb();
}

void MainWindow::left_side_cb() {
    Fl_Tree_Item* item = left_side_choice->callback_item();
    Fl_Tree_Reason reason = left_side_choice->callback_reason();
    void* v = item->user_data();

    if (v == SIDE_MANUAL && reason == FL_TREE_REASON_SELECTED) {
        bl_in->show();
        cl_in->show();
    }
    else {
        bl_in->hide();
        cl_in->hide();
    }
}

void MainWindow::right_side_cb_st(Fl_Widget*, void* v) {
    MainWindow* w = (MainWindow*)v;
    w->right_side_cb();
}

void MainWindow::right_side_cb() {
    Fl_Tree_Item* item = right_side_choice->callback_item();
    Fl_Tree_Reason reason = right_side_choice->callback_reason();
    void* v = item->user_data();

    if (v == SIDE_MANUAL && reason == FL_TREE_REASON_SELECTED) {
        br_in->show();
        cr_in->show();
    }
    else {
        br_in->hide();
        cr_in->hide();
    }
}
