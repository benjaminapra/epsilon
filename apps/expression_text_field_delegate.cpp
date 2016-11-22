#include "expression_text_field_delegate.h"
#include "apps_container.h"
#include <math.h>
#include <string.h>

const char * ExpressionTextFieldDelegate::XNT() {
  return "x";
}

bool ExpressionTextFieldDelegate::cursorInToken(TextField * textField, const char * token) {
  const char * text = textField->text();
  int cursorLocation = textField->cursorLocation();
  int tokenLength = strlen(token);
  if (cursorLocation - tokenLength < 0) {
    return false;
  }
  char previousToken[10];
  strlcpy(previousToken, text+cursorLocation-tokenLength, tokenLength+1);
  if (strcmp(previousToken, token) == 0) {
    return true;
  }
  return false;
}

bool ExpressionTextFieldDelegate::textFieldDidReceiveEvent(TextField * textField, Ion::Events::Event event) {
  if (event == Ion::Events::OK && Expression::parse(textField->text()) == nullptr) {
    if (textField->textLength() == 0) {
      return true;
    }
    textField->app()->displayWarning("Attention a la syntaxe jeune padawan");
    return true;
  }
  if (event == Ion::Events::OK &&
    isnan(Expression::parse(textField->text())->approximate(*evaluateContext()))) {
      textField->app()->displayWarning("Relis ton cours de maths, veux tu?");
      return true;
  }
  if (event == Ion::Events::Toolbox) {
    AppsContainer * appsContainer = (AppsContainer *)textField->app()->container();
    ToolboxController * toolboxController = appsContainer->toolboxController();
    toolboxController->setTextFieldCaller(textField);
    textField->app()->displayModalViewController(toolboxController, 0.f, 0.f, 50, 50, 0, 50);
    return true;
  }
  if (event == Ion::Events::Var) {
    AppsContainer * appsContainer = (AppsContainer *)textField->app()->container();
    VariableBoxController * variableBoxController = appsContainer->variableBoxController();
    variableBoxController->setTextFieldCaller(textField);
    textField->app()->displayModalViewController(variableBoxController, 0.f, 0.f, 50, 50, 0, 50);
    return true;
  }
  if (event == Ion::Events::XNT) {
    if (cursorInToken(textField, "sum(") || cursorInToken(textField, "product(")) {
      textField->insertTextAtLocation("n", textField->cursorLocation());
      textField->setCursorLocation(textField->cursorLocation()+strlen("n"));
      return true;
    }
    textField->insertTextAtLocation(XNT(), textField->cursorLocation());
    textField->setCursorLocation(textField->cursorLocation()+strlen(XNT()));
    return true;
  }
  return false;
}